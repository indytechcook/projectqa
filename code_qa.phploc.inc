<?php

/**
 * @file Logic pertaining to PHPLOC
 */


define ('PHPLOC', DRUPAL_ROOT . '/sites/all/libraries/phploc.phar');


/**
 * Detects whether or not phploc is available
 */
function code_qa_detect_phploc() {
  $command = PHPLOC . ' ./';

  $r = exec($command);

  if( exec($command) ) {
    return TRUE;
  }
  return FALSE;
}


/**
 * Downloads the phploc.phar library
 */
function code_qa_download_phploc() {
  // $dir = DRUPAL_ROOT . '/' . drupal_get_path('module', 'code_qa');
  //
  // exec('wget -P ' . $dir . ' https://phar.phpunit.de/phploc.phar');
  // exec('chmod a+x ' . $dir . '/phploc.phar');
  
}


/**
 *
 */
function code_qa_run_phploc() {
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

  // TODO: Don't clear out records
  // Temporarily clear out the db and start over each pageload
  db_delete('phploc_result')->execute();

  code_qa_generate_phploc_resultset();

  $table = code_quality_generate_phploc_result_table();

  $form['table'] = $table;

  return $form;
}


/**
 * Generates the PHPLOC result set
 */
function code_qa_generate_phploc_resultset() {
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
    $t = exec(PHPLOC . $params . $output_file . $module_path);

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
     // 'directories' => t('Directories'),
     // 'files' => t('Files'),
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
  
  $keys = array(
    'directories' => t('Directories'),
    'files' => t('Files'),
  );

  $keys += array_keys(code_qa_get_phploc_result_map());
    
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
  $results = db_select('phploc_result', 'pr')
    ->fields('pr')
    ->execute()
    ->fetchAllAssoc('prid', PDO::FETCH_ASSOC);

  $header = array(
    'module' => t('Module'),
    'timestamp' => t('Timestamp'),
  );

  $header += code_qa_get_phploc_result_map();


  $results_table['table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $results,
    '#multiple' => FALSE,
  );

  return $results_table;
}


