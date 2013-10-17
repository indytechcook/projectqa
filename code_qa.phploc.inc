<?php

/**
 * @file Logic pertaining to PHPLOC
 */

/**
 *
 */
function code_qa_run_phploc() {
  code_qa_generate_phploc_resultset();
  drupal_set_message(t('The PHPLOC tests have been run'));
  drupal_goto('admin/code_qa');
}


/**
 *
 */
function code_qa_view_phploc() {
  drupal_goto('admin/code_qa/phploc');
}


/**
 * Creates the PHPLOC report page
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
 * Generates the PHPLOC result set
 */
function code_qa_generate_phploc_resultset() {
  composer_manager_register_autoloader();

  $phploc = composer_manager_vendor_dir() . "/bin/phploc";

  $module_list = module_list();

  $params = ' --names *.php,*.info,*.module,*.install,*.inc --log-csv ';
  $tmp_directory = file_directory_temp();

  foreach ($module_list as $module_name) {
    $skip_core_modules = FALSE;
    $module_path = DRUPAL_ROOT . '/' . drupal_get_path('module', $module_name);
    $core_module_path = DRUPAL_ROOT . '/modules';

    if ($skip_core_modules && strncmp($module_path, $core_module_path, strlen($core_module_path))) {
      continue; // Skip core modules
    }

    // Run phploc on the module
    $output_file = $tmp_directory . '/' . $module_name . '.csv ';
    $t = exec($phploc . $params . $output_file . $module_path);

    $form['list'][] = array(
      '#type' => 'markup',
      '#markup' => $module_name . ' - ' . $output_file,
      '#prefix' => '<li>',
      '#suffix' => '</li>',
    );

    // Read in the phploc values and insert into DB
    $tmp_file = trim($output_file);
    $lines = file($tmp_file);

    code_qa_save_phploc_resultset($module_name, $lines[1]);
  }
}


/**
 * Creates a set of key/values for data columns
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
 * Saves the PHPLOC result as an entity
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
 * Generate result table
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



  //TODO: Can't do LIMIT in a subquery in MySQL?
  $subresult = db_query('SELECT MAX(prid) as prid FROM phploc_result GROUP BY module ORDER BY MAX(prid) DESC');
  $subresult = $subresult->fetchAllAssoc('prid');

  $ids = array_keys($subresult);

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
 *
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

  $form[] = code_qa_phploc_files_dir_history_graph($module_name);
  $form[] = code_qa_phploc_cc_history_graph($module_name);

  return $form;
}


function code_qa_phploc_files_dir_history_graph($module_name) {
  $widget = array(
    '#type' => 'fieldset',
    '#title' => t('Files & Directories History'),
    '#tree' => TRUE,
    '#attributes' => array(
      'style' => array(
        'display: inline-block;',
        'margin-right: 30px;',
      )
    ),
  );

  $widget[] = array(
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

  foreach($result as $row) {
    $timestamps[] = format_date($row->timestamp, $type = 'short');
    $files[] = $row->files;
    $dirs[] = $row->directories;
  }


  $settings['chart']['files_dirs'] = array(
    'header' => $timestamps,
    'rows' => array($files, $dirs),
    'columns' => array(t('Files'), t('Directories')),
    'chartType' => 'LineChart',
    'containerId' =>  'files_dir_container',
    'options' => array(
      'title' => t('Files & Directories History'),
      'titlePosition' => 'none',
      'width' => 500,
      'height' => 300,
      'curveType' => 'none',
      'hAxis' => array(
        'slantedText' => TRUE,
      ),
      'pointSize' => 5,
    )
  );

  //Draw it.
  $ret = draw_chart($settings);


  return $widget;
}


function code_qa_phploc_cc_history_graph($module_name) {
  $widget = array(
    '#type' => 'fieldset',
    '#title' => t('Cyclomatic Complexity History'),
    '#tree' => TRUE,
    '#attributes' => array(
      'style' => array(
        'display: inline-block;',
        'margin-right: 0px;',
      )
    ),
  );

  $widget[] = array(
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

  foreach($result as $row) {
    $timestamps[] = format_date($row->timestamp, $type = 'short');
    $cc_loc[] = $row->cc_loc;
    $cc_methods[] = $row->cc_methods;
  }


  $settings['chart']['cc'] = array(
    'header' => $timestamps,
    'rows' => array($cc_loc, $cc_methods),
    'columns' => array(t('LOC'), t('Methods')),
    'chartType' => 'LineChart',
    'containerId' =>  'cc_container',
    'options' => array(
      'title' => t('Cyclomatic Complexity History'),
      'titlePosition' => 'none',
      'width' => 500,
      'height' => 300,
      'curveType' => 'none',
      'hAxis' => array(
        'slantedText' => TRUE,
      ),
      'pointSize' => 5,
    )
  );

  //Draw it.
  $ret = draw_chart($settings);


  return $widget;
}


