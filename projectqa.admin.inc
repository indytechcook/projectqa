<?php
/**
 * @file
 * Admin pages and functions
 */


/**
 * Renders the project list summary.
 *
 * @param array $form
 *   The form to add the project list to.
 */
function projectqa_repo_list(&$form) {

  $header = array(

    array(
      'data' => t('Name'),
      'field' => 'p.name',
    ),
    array(
      'data' => t('URL'),
      'field' => 'p.url',
    ),
    array(
      'data' => t('Branch'),
      'field' => 'p.branch',
    ),
  );

  $projects = projectqa_get_projects();

  foreach ($projects as $record) {
    $rows[] = array(
      $record->pid,
      $record->name,
      $record->url,
      $record->branch,
    );
  }

  if (!isset($rows)) {
    drupal_set_message(t('No projects have been added.'));
    drupal_set_message(t('For now, to add/remove projects you need to edit the projectqa_repo table.'));
    return;
  }

  $form['projects'] = array(
    '#type' => 'fieldset',
    '#title' => t('Projects'),
    '#tree' => TRUE,
  );

  $form['projects']['tablesort_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
}
