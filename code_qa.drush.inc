<?php
/**
 * @file
 * Drush integration implementation
 */

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
 */
function drush_code_qa_phploc($arg1 = NULL) {
  drush_log('Running phploc', 'ok');
  code_qa_generate_phploc_resultset();
}
