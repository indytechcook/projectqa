<?php
/**
 * @file
 * Drush integration implementation
 */

/**
 * Implements hook_drush_help().
 */
function project_qa_drush_help($command) {
  switch ($command) {
    case 'drush:pqa':
      return dt('Run Project QA');
  }
}

/**
 * Implements hook_drush_command().
 */
function project_qa_drush_command() {
  $items = array();
  $items['pqa'] = array(
    'description' => dt('Run Project QA.'),
    'examples' => array(
      'Standard example' => 'drush pqa',
    ),
  );
  return $items;
}


/**
 * Callback function for drush phploc.
 */
function drush_project_qa_pqa($arg1 = NULL) {
  project_qa_walk_projects();
}
