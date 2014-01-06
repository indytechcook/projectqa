<?php
/**
 * @file
 * Drush integration implementation
 */

/**
 * Implements hook_drush_help().
 */
function projectqa_drush_help($command) {
  switch ($command) {
    case 'drush:pqa':
      return dt('Run ProjectQA');
  }
}

/**
 * Implements hook_drush_command().
 */
function projectqa_drush_command() {
  $items = array();
  $items['pqa'] = array(
    'description' => dt('Run ProjectQA.'),
    'examples' => array(
      'Standard example' => 'drush pqa',
    ),
  );
  return $items;
}


/**
 * Callback function for drush phploc.
 */
function drush_projectqa_pqa($arg1 = NULL) {
  projectqa_walk_projects();
}
