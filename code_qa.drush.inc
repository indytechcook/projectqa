<?php

/**
 * Implements hook_drush_help().
 */
function code_qa_drush_help($command) {
  switch ($command) {
    case 'drush:phploc':
      return dt('Run PHPLOC');
  }
}

/**
 * Implements hook_drush_command().
 */
function code_qa_drush_command() {
  $items = array();
  $items['phploc'] = array(
    'description' => dt('Run PHPLOC.'),
    'examples' => array(
      'Standard example' => 'drush phploc',
    ),
    'aliases' => array('loc'),
  );
  return $items;
}


/**
 * Callback function for drush phploc.
 * Callback is called by using drush_hook_command() where
 * hook is the name of the module (code_qa) and command is the name of
 * the Drush command with all "-" characters converted to "_" characters (my_command)
 *
 * @param $arg1
 *   An optional argument
 */
function drush_code_qa_phploc($arg1 = NULL) {
  //check if the argument was passed in and just print it out
  if (isset($arg1)) {
   drush_print($arg1);
  }

  drush_log('Running phploc', 'ok');
  code_qa_generate_phploc_resultset();
}
