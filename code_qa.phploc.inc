<?php
/**
 * @file
 * Logic pertaining to PHPLOC
 */


use SebastianBergmann\FinderFacade\FinderFacade;
use SebastianBergmann\Git;
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
      code_qa_execute_git($repo_path, 'checkout master');
    }
    else {
      // Get the repo.
      exec('git clone ' . $project->url . ' ' . $repo_path);
    }

    // Execute PHPLOC on repo.
    code_qa_phploc_analyize($project);

  }

}


/**
 * Creates a set of key/values for data columns.
 */
function code_qa_get_phploc_result_map() {
  $map = array(
    'directories' => t('(Sub) Directories'),
    'files' => t('Files'),
    'loc' => t('Lines of Code'),
    'cc_loc' => t('Complexity/Lines'),
    'commented_loc' => t('Commented Lines of Code'),
    'non_commented_loc' => t('Non-Commented Lines of Code'),
    'lloc' => t('Logical Lines of Code'),
    'namespaces' => t('Namespaces'),
    'interfaces' => t('Interfaces'),
    'traits' => t('Traits'),
    'classes' => t('Classes'),
    'abstract_classes' => t('Abstract Classes'),
    'concrete_classes' => t('Concrete Classes'),
    'avg_class_length' => t('Avg Class Length'),
    'methods' => t('Methods'),
    'non_static_methods' => t('Non-Static Methods'),
    'static_methods' => t('Static Methods'),
    'public_methods' => t('Public Methods'),
    'non_public_methods' => t('Non-public Methods'),
    'avg_method_length' => t('Avg Method Length'),
    'cc_methods' => t('Complexity/Methods'),
    'functions' => t('Functions'),
    'named_functions' => t('Named Functions'),
    'anonymous_functions' => t('Anonymous Functions'),
    'constants' => t('Constants'),
    'global_constants' => t('Global Constants'),
    'class_constants' => t('Class Constants'),
    'attribute_accesses' => t('Attribute Accesses'),
    'non_static_attribute_accesses' => t('Non-Static Attribute Accesses'),
    'static_attribute_accesses' => t('Static Attribute Accesses'),
    'method_calls' => t('Method Calls'),
    'non_static_method_calls' => t('Non-Static Method Calls'),
    'static_method_calls' => t('Static Method Calls'),
    'global_accesses' => t('Global Accesses'),
    'global_variable_accesses' => t('Global Variable Accesses'),
    'super_global_var_accesses' => t('Super Global Variable Accesses'),
    'global_constant_accesses' => t('Global Constant Accesses'),
    'test_classes' => t('Test Classes'),
    'test_methods' => t('Test Methods'),
  );

  return $map;
}


/**
 * Saves the PHPLOC result as an entity.
 */
function code_qa_save_phploc_resultset($module_name, $data) {

  $keys = array_keys(code_qa_get_phploc_result_map());

  $values = explode(',', $data);

  $phploc_data = array_combine($keys, $values);
  $phploc_data['module'] = $module_name;
  $phploc_data['timestamp'] = time();

  $entity = entity_create('phploc_result', $phploc_data);
  $entity->save();
}


/**
 * Generate result table.
 */
function code_quality_generate_phploc_result_table() {

  $results_map = code_qa_get_phploc_result_map();
  $header = array(
    array(
      'data' => t('Module'),
      'field' => 'pr.module',
    ),
    array(
      'data' => $results_map['directories'],
      'field' => 'pr.directories',
    ),
    array(
      'data' => $results_map['files'],
      'field' => 'pr.files',
    ),
    array(
      'data' => $results_map['loc'],
      'field' => 'pr.loc',
    ),
    array(
      'data' => $results_map['lloc'],
      'field' => 'pr.lloc',
    ),
    array(
      'data' => $results_map['commented_loc'],
      'field' => 'pr.commented_loc',
    ),
    array(
      'data' => $results_map['cc_loc'],
      'field' => 'pr.cc_loc',
    ),
    array(
      'data' => $results_map['methods'],
      'field' => 'pr.methods',
    ),
    array(
      'data' => $results_map['cc_methods'],
      'field' => 'pr.cc_methods',
    ),
    array(
      'data' => $results_map['method_calls'],
      'field' => 'pr.method_calls',
    ),
    array(
      'data' => $results_map['constants'],
      'field' => 'pr.constants',
    ),
    array(
      'data' => $results_map['global_constants'],
      'field' => 'pr.global_constants',
    ),
  );

  // TODO: Can't do LIMIT in a subquery in MySQL?
  $subresult = db_query('SELECT MAX(prid) as prid FROM phploc_result GROUP BY module ORDER BY MAX(prid) DESC');
  $subresult = $subresult->fetchAllAssoc('prid');

  $ids = array_keys($subresult);

  if ($ids != NULL) {
    $query = db_select('phploc_result', 'pr')
      ->fields('pr',
        array(
          'module',
          'directories',
          'files',
          'loc',
          'lloc',
          'commented_loc',
          'cc_loc',
          'methods',
          'cc_methods',
          'method_calls',
          'constants',
          'global_constants',
        ))
      ->condition('prid', $ids, 'IN')
      ->extend('TableSort');
    $result = $query->orderByHeader($header)->execute();
  }
  else {
    $result = array();
  }

  $header += array(
    array(
      'data' => 'test',
      'field' => 'test',
    ),
  );

  $rows = array();
  foreach ($result as $row) {
    $row->module = l($row->module, 'admin/code_qa/phploc/' . $row->module);
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
function code_qa_phploc_report_module_page($form, &$form_state, $module_name) {
  $form = array();

  $form['module_name'] = array(
    '#type' => 'markup',
    '#markup' => t('Module: @mod', array('@mod' => $module_name)),
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
  $overview[] = code_qa_phploc_files_dir_history_graph($module_name);
  $overview[] = code_qa_phploc_cc_history_graph($module_name);
  $form[] = $overview;

  $size = array(
    '#type' => 'fieldset',
    '#title' => t('Code Size'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
  );
  $size[] = code_qa_phploc_code_comments_graph($module_name);
  $size[] = code_qa_phploc_classes_function_graph($module_name);
  $form[] = $size;

  $dependecies = array(
    '#type' => 'fieldset',
    '#title' => t('Dependencies'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
  );
  $form[] = $dependecies;

  $structure = array(
    '#type' => 'fieldset',
    '#title' => t('Structure'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
  );
  $form[] = $structure;

  return $form;
}


/**
 * Loads data and generates the history graph files and directories.
 *
 * @param string $module_name
 *   The name of the module to evaluate.
 *
 * @return array
 *   The Form API array for this graph
 */
function code_qa_phploc_files_dir_history_graph($module_name) {
  $container[] = array(
    '#type' => 'container',
    '#id' => 'files_dir_container',
  );

  $query = db_select('phploc_result', 'pr')
    ->fields('pr', array('timestamp', 'directories', 'files'))
    ->condition('pr.module', $module_name, '=')
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
      'width' => 500,
      'height' => 200,
      'curveType' => 'none',
      'legend' => 'none',
      'hAxis' => array(
        'slantedText' => TRUE,
      ),
      'pointSize' => 5,
    ),
  );

  // Draw it.
  $ret = draw_chart($settings);

  return $container;
}


/**
 * Loads data and generates the history graph for cyclomatic complexity.
 *
 * @param string $module_name
 *   The name of the module to evaluate.
 *
 * @return array
 *   The Form API array for this graph
 */
function code_qa_phploc_cc_history_graph($module_name) {
  $container[] = array(
    '#type' => 'container',
    '#id' => 'cc_container',
  );

  $query = db_select('phploc_result', 'pr')
    ->fields('pr', array('timestamp', 'cc_loc', 'cc_methods'))
    ->condition('pr.module', $module_name, '=')
    ->orderBy('timestamp', 'ASC');

  $result = $query->execute()->fetchAllAssoc('timestamp');

  $result = $query->execute()->fetchAllAssoc('timestamp');
  $timestamps = array();
  $cc_loc = array();
  $cc_methods = array();

  foreach ($result as $row) {
    $timestamps[] = format_date($row->timestamp, $type = 'short');
    $cc_loc[] = $row->cc_loc;
    $cc_methods[] = $row->cc_methods;
  }

  $settings['chart']['cc'] = array(
    'header' => $timestamps,
    'rows' => array($cc_loc, $cc_methods),
    'columns' => array(t('LOC'), t('Methods')),
    'chartType' => 'LineChart',
    'containerId' => 'cc_container',
    'options' => array(
      'title' => t('Cyclomatic Complexity History'),
      'width' => 500,
      'height' => 200,
      'curveType' => 'none',
      'legend' => 'none',
      'hAxis' => array(
        'slantedText' => TRUE,
      ),
      'pointSize' => 5,
    ),
  );

  // Draw it.
  $ret = draw_chart($settings);

  return $container;
}


/**
 * Loads data and generates the graph for code and comments.
 *
 * @param string $module_name
 *   The name of the module to evaluate.
 *
 * @return array
 *   The Form API array for this graph
 */
function code_qa_phploc_code_comments_graph($module_name) {
  $container[] = array(
    '#type' => 'container',
    '#id' => 'code_comments_container',
    '#attributes' => array(
      'style' => array(
        'display:block;',
      ),
    ),
  );

  $query = db_select('phploc_result', 'pr')
    ->fields('pr', array('timestamp', 'commented_loc', 'non_commented_loc'))
    ->condition('pr.module', $module_name, '=')
    ->orderBy('timestamp', 'ASC')
    ->range(0, 1);

  $result = $query->execute()->fetchAssoc();

  $settings['chart']['code_comments'] = array(
    'header' => array('Commented', 'Non-Commented'),
    'rows' => array(array($result['commented_loc'], $result['non_commented_loc'])),
    'columns' => array('Code Comments'),
    'chartType' => 'PieChart',
    'containerId' => 'code_comments_container',
    'options' => array(
      'title' => t('Code Comments'),
      'width' => 300,
      'height' => 300,
      'curveType' => 'none',
      'legend' => 'none',
      'hAxis' => array(
        'slantedText' => TRUE,
      ),
      'is3D' => TRUE,
      'pointSize' => 5,
    ),
  );

  // Draw it.
  $ret = draw_chart($settings);

  return $container;
}

/**
 * Loads data and generates the graph for classes & functions.
 *
 * @param string $module_name
 *   The name of the module to evaluate.
 *
 * @return array
 *   The Form API array for this graph
 */
function code_qa_phploc_classes_function_graph($module_name) {
  $container = array(
    '#type' => 'container',
    '#id' => 'classes_functions_container',
    '#attributes' => array(
      'style' => array(
        'display:inline;',
      ),
    ),
  );

  $query = db_select('phploc_result', 'pr')
    ->fields('pr', array('timestamp', 'lloc', 'classes', 'functions'))
    ->condition('pr.module', $module_name, '=')
    ->orderBy('timestamp', 'ASC')
    ->range(0, 1);

  $result = $query->execute()->fetchAssoc();

  $other = $result['lloc'] - $result['classes'] - $result['functions'];

  $settings['chart']['classes_functions'] = array(
    'header' => array('Classes', 'Functions', 'Other'),
    'rows' => array(array($result['classes'], $result['functions'], $other)),
    'columns' => array('Classes & Functions'),
    'chartType' => 'PieChart',
    'containerId' => 'classes_functions_container',
    'options' => array(
      'title' => t('Classes & Functions'),
      'width' => 300,
      'height' => 300,
      'curveType' => 'none',
      'legend' => 'none',
      'hAxis' => array(
        'slantedText' => TRUE,
      ),
      'is3D' => TRUE,
      'pointSize' => 5,
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

  // Get the last hash recorded for this project (if it exists).
  $query = db_select('phploc_result', 'pr')
    ->fields('pr',
      array(
        'pid',
        'timestamp',
        'sha1',
      )
    )
    ->condition('pid', $project->pid, '=')
    ->orderBy('timestamp', 'DESC')
    ->range(0, 1);

  $result = $query->execute();
  $old_hash = $result->fetchField(2);

  $log_options = array();

  if ($old_hash) {
    $new_hash = code_qa_execute_git(
      $project_path,
      'log',
      array(
        '-n 1',
        '--pretty=format:"%h"',
        'HEAD~',
      )
    );

    $log_options[] = $old_hash . '..' . $new_hash[0];
  }
  $log_options[] = '--no-merges';
  $log_options[] = '--date-order';
  $log_options[] = '--reverse';
  $log_options[] = '--format=medium';

  $log = code_qa_execute_git($project_path, 'log', $log_options);
  $revisions = code_qa_process_git_revisions($log);

  $previous_data = array();

  foreach ($revisions as $revision) {
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
      'master',
    )
  );
}
