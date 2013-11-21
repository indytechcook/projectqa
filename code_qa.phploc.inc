<?php
/**
 * @file
 * Logic pertaining to PHPLOC
 */


use SebastianBergmann\FinderFacade\FinderFacade;
use SebastianBergmann\PHPLOC\Analyser;

/**
 * Callback for the Run PHPLOC button.
 */
function code_qa_run_phploc() {
  code_qa_generate_phploc_resultset();
  drupal_set_message(t('The PHPLOC tests have been run'));
  drupal_goto('admin/code_qa');
}


/**
 * Menu callback.
 */
function code_qa_view_phploc() {
  drupal_goto('admin/code_qa/phploc');
}


/**
 * Creates the PHPLOC report page.
 */
function code_qa_phploc_report_page($form, &$form_state) {
  $form = array();

  $form['list'] = array(
    '#type' => 'markup',
    '#markup' => '',
    '#prefix' => '<ul>',
    '#suffix' => '</ul>',
  );

  $table = code_quality_generate_phploc_result_table();
  $form['table'] = $table;

  return $form;
}


/**
 * Generates the PHPLOC result set.
 */
function code_qa_generate_phploc_resultset() {
  $projects = code_qa_get_projects();
  $filepath = code_qa_get_filepath();

  foreach ($projects as $project) {
    $repo_path = $filepath . '/' . $project->name;

    // Check for exisisting copy of the repo.
    if (file_exists($repo_path)) {
      // Update repo.
      code_qa_execute_git($repo_path, 'pull');
    }
    else {
      // Get the repo.
      exec('git clone ' . $project->url . ' ' . $repo_path);
    }

    code_qa_execute_git($repo_path, 'checkout ' . $project->branch);

    // Execute PHPLOC on repo.
    code_qa_phploc_analyize($project);

  }

}


/**
 * Generate result table.
 */
function code_quality_generate_phploc_result_table() {

  $header = array(
    array(
      'data' => t('Name'),
      'field' => 'p.name',
    ),
    array(
      'data' => t('Directories'),
      'field' => 'pr.directories',
    ),
    array(
      'data' => t('Files'),
      'field' => 'pr.files',
    ),
    array(
      'data' => t('Lines of Code'),
      'field' => 'pr.loc',
    ),
    array(
      'data' => t('Logical Lines of Code'),
      'field' => 'pr.lloc',
    ),
    array(
      'data' => t('Commented Lines of Code'),
      'field' => 'pr.cloc',
    ),
    array(
      'data' => t('CC/LLOC'),
      'field' => 'pr.ccnByLloc',
    ),
    array(
      'data' => t('Method Calls'),
      'field' => 'pr.methodCalls',
    ),
    array(
      'data' => t('CC/Number of Methods'),
      'field' => 'pr.ccnByNom',
    ),
  );

  // TODO: Can't do LIMIT in a subquery in MySQL?
  $subresult = db_query('SELECT MAX(prid) as prid FROM phploc_result GROUP BY pid ORDER BY MAX(prid) DESC');
  $subresult = $subresult->fetchAllAssoc('prid');

  $ids = array_keys($subresult);

  if ($ids != NULL) {
    $query = db_select('phploc_result', 'pr')
      ->fields('pr',
        array(
          'pid',
          'directories',
          'files',
          'loc',
          'lloc',
          'cloc',
          'ccnByLloc',
          'methodCalls',
          'ccnByNom',
        ))
      ->condition('prid', $ids, 'IN')
      ->extend('TableSort');
    $query->join('code_qa_project', 'p', 'pr.pid = p.pid');
    $query->fields('p',
      array(
        'name',
      ));
    $result = $query->orderByHeader($header)->execute();
  }
  else {
    $result = array();
  }


  $rows = array();
  foreach ($result as $row) {
    $row->pid = l($row->name, 'admin/code_qa/phploc/' . $row->pid);
    unset($row->name);
    $rows[] = array('data' => (array) $row);
  }

  $results_table['tablesort_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  return $results_table;
}


/**
 * Callback to generate the module report page.
 *
 * @param array $form
 *   The form array for this page.
 *
 * @param array $form_state
 *   The form status info.
 *
 * @param string $module_name
 *   The name of the module to be processed.
 *
 * @return array
 *   The Form API array representing this page.
 */
function code_qa_phploc_report_module_page($form, &$form_state, $pid) {
  $query = db_select('code_qa_project', 'p')
    ->fields('p', array('pid', 'name', 'url', 'branch'))
    ->condition('p.pid', $pid, '=');

  $result = $query->execute();
  $project = $result->fetchObject();
  $form = array();

  $form['module_name'] = array(
    '#type' => 'markup',
    '#markup' => t('Project: @proj', array('@proj' => $project->name)),
    '#prefix' => '<h2>',
    '#suffix' => '</h2>',
  );

  $form['back_link'] = array(
    '#type' => 'markup',
    '#markup' => l(t('Back to summary'), 'admin/code_qa/phploc') ,
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );

  $overview = array(
    '#type' => 'fieldset',
    '#title' => t('Overview'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
  );

  //TODO: Update these to handle passing the project.
  $overview[] = code_qa_phploc_files_dir_history_graph($pid);
  $overview[] = code_qa_phploc_cc_history_graph($pid);
  $form[] = $overview;

  return $form;
}


/**
 * Loads data and generates the history graph files and directories.
 */
function code_qa_phploc_files_dir_history_graph($pid) {
  $container[] = array(
    '#type' => 'container',
    '#id' => 'files_dir_container',
  );

  $query = db_select('phploc_result', 'pr')
    ->fields('pr', array('timestamp', 'directories', 'files'))
    ->condition('pr.pid', $pid, '=')
    ->orderBy('timestamp', 'ASC');

  $result = $query->execute()->fetchAllAssoc('timestamp');
  $timestamps = array();
  $files = array();
  $dirs = array();

  foreach ($result as $row) {
    $timestamps[] = format_date($row->timestamp, $type = 'short');
    $files[] = $row->files;
    $dirs[] = $row->directories;
  }

  $settings['chart']['files_dirs'] = array(
    'header' => $timestamps,
    'rows' => array($files, $dirs),
    'columns' => array(t('Files'), t('Directories')),
    'chartType' => 'LineChart',
    'containerId' => 'files_dir_container',
    'options' => array(
      'title' => t('Files & Directories History'),
      'width' => 1000,
      'height' => 200,
      'curveType' => 'none',
      'hAxis' => array(
        'slantedText' => TRUE,
      ),
      'pointSize' => 0,
      'tooltip' => array(
        'trigger' => 'selection',
      ),
      'selectionMode' => 'multiple',
    ),
  );

  // Draw it.
  $ret = draw_chart($settings);

  return $container;
}


function code_qa_phploc_cc_history_graph($pid) {
  $container[] = array(
    '#type' => 'container',
    '#id' => 'cc_container',
  );

  $query = db_select('phploc_result', 'pr')
    ->fields('pr', array('timestamp', 'ccnByLloc', 'ccnByNom'))
    ->condition('pr.pid', $pid, '=')
    ->orderBy('timestamp', 'ASC');

  $result = $query->execute()->fetchAllAssoc('timestamp');

  $result = $query->execute()->fetchAllAssoc('timestamp');
  $timestamps = array();
  $cc_loc = array();
  $cc_methods = array();

  foreach ($result as $row) {
    $timestamps[] = format_date($row->timestamp, $type = 'short');
    $ccnByLloc[] = $row->ccnByLloc;
    $ccnByNom[] = $row->ccnByNom;
  }

  $settings['chart']['cc'] = array(
    'header' => $timestamps,
    'rows' => array($ccnByLloc),
    'columns' => array(t('CC/LLOC')),
    'chartType' => 'LineChart',
    'containerId' => 'cc_container',
    'options' => array(
      'title' => t('Cyclomatic Complexity History'),
      'width' => 1000,
      'height' => 200,
      'curveType' => 'none',
      'hAxis' => array(
        'slantedText' => TRUE,
      ),
      'pointSize' => 0,
      'tooltip' => array(
        'trigger' => 'selection',
      ),
      'selectionMode' => 'multiple',
    ),
  );

  // Draw it.
  $ret = draw_chart($settings);

  return $container;
}



/**
 * Kick off the analasys of the code.
 *
 * @param object $project
 *   The project object of the code to evaluate.
 */
function code_qa_phploc_analyize($project) {
  composer_manager_register_autoloader();
  $project_path = code_qa_get_filepath() . '/' . $project->name;

  $excludes = array();
  $names = array(
    '*.php',
    '*.info',
    '*.module',
    '*.install',
    '*.inc',
  );
  $names_exclude = array();
  $log_options = array();

  // Get the last hash recorded for this project (if it exists).
  $query = db_select('phploc_result', 'pr')
    ->fields('pr')
    ->condition('pid', $project->pid, '=')
    ->orderBy('timestamp', 'DESC')
    ->range(0, 1);
  $result = $query->execute();
  $previous_data = $result->fetchAssoc();

  if ($previous_data) {
    $new_hash = code_qa_execute_git(
      $project_path,
      'log',
      array(
        '-n 1',
        '--pretty=format:"%h"',
        'HEAD',
      )
    );

    $log_options[] = $previous_data['sha1'] . '..' . $new_hash[0];
  }

  $log_options[] = '--no-merges';
  $log_options[] = '--date-order';
  $log_options[] = '--reverse';
  $log_options[] = '--format=medium';
  $log_options[] = $project->branch;

  $log = code_qa_execute_git($project_path, 'log', $log_options);
  $revisions = code_qa_process_git_revisions($log);

  $revisions_num = count($revisions);
  $iteration_counter = 0;
  foreach ($revisions as $revision) {
    if ($iteration_counter % 50 == 0) {
      echo 'Processing ' . $iteration_counter . ' of ' . $revisions_num . "\n";
    }
    $iteration_counter++;
    // Check out this commit.
    code_qa_execute_git(
      $project_path,
      'checkout',
      array(
        '--force',
        '--quiet',
        $revision['sha1'],
        '2>&1',
      )
    );

    // Look for files to evaluate.
    $directories = array();
    $directory = realpath($project_path);
    if ($directory) {
      $directories[] = $directory;
    }
    $finder = new FinderFacade($directories, $excludes, $names, $names_exclude);
    $files  = $finder->findFiles();

    // Eval the code.
    $analyser = new Analyser();
    $data = $analyser->countFiles($files, TRUE);

    // Generate the delta values.
    foreach (array_keys($data) as $key) {

      $previous_value = ($previous_data[$key] ? $previous_data[$key] : 0);
      $delta = $data[$key] - $previous_value;

      $delta_key = $key . '_delta';
      $data[$delta_key] = $delta;
    }

    $entity = entity_create('phploc_result', $data);

    // Get the author info for this commit.
    $author = code_qa_execute_git(
      $project_path,
      'log',
      array(
        '-n 1',
        '--pretty=format:"%ae"',
        $revision['sha1'],
      )
    );

    // Set the extra data.
    $entity->author = $author[0];
    $entity->pid = $project->pid;
    $entity->sha1 = $revision['sha1'];
    $entity->timestamp = $revision['date']->getTimestamp();

    $entity->save();

    // Set up the previous data array for the next loop.
    $previous_data = $data;
  }

  // Reset the repo back to HEAD.
  code_qa_execute_git(
    $project_path,
    'checkout',
    array(
      '--force',
      '--quiet',
      $project->branch,
    )
  );
}
