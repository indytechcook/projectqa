<?php
/**
 * @file
 * Drush integration implementation
 */

/**
 * Implements hook_drush_help().
 */
function phploc_drush_help($command) {
  switch ($command) {
    case 'drush:phploc':
      return dt('Run PHPLOC');
  }
}

/**
 * Implements hook_drush_command().
 */
function phploc_drush_command() {
  $items = array();
  $items['phploc'] = array(
    'description' => dt('Run PHPLOC.'),
    'examples' => array(
      'Standard example' => 'drush phploc',
    ),
  );
  return $items;
}


/**
 * Callback function for drush phploc.
 */
function drush_phploc($arg1 = NULL) {
  drush_log('Running phploc', 'ok');
  phploc_generate_phploc_resultset();
}
