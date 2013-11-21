<?php
/**
 * @file
 * Admin pages and functions
 */

/**
 * Callback for module page.
 */
function code_qa_admin_page($form, &$form_state) {
  $form = array();

  code_qa_project_list($form);

  $form['phploc'] = array(
    '#type' => 'fieldset',
    '#title' => t('PHPLOC'),
    '#tree' => TRUE,
  );

  $phploc_actions = array();

  // $phploc_actions['run'] = array(
  //   '#type' => 'submit',
  //   '#value' => t('Run PHPLOC'),
  //   '#submit' => array('code_qa_run_phploc'),
  // );
  $phploc_actions['view'] = array(
    '#type' => 'submit',
    '#value' => t('View results'),
    '#submit' => array('code_qa_view_phploc'),
  );

  $form['phploc']['action'] = $phploc_actions;

  return $form;
}


/**
 * Renders the project list summary.
 *
 * @param array $form
 *   The form to add the project list to.
 */
function code_qa_project_list(&$form) {
  $form['projects'] = array(
    '#type' => 'fieldset',
    '#title' => t('Projects'),
    '#tree' => TRUE,
  );

  $header = array(
    array(
      'data' => t('ID'),
      'field' => 'p.pid',
    ),
    array(
      'data' => t('Name'),
      'field' => 'p.name',
    ),
    array(
      'data' => t('URL'),
      'field' => 'p.url',
    ),
  );

  $projects = code_qa_get_projects();

  foreach ($projects as $record) {
    $rows[] = array(
      $record->pid,
      $record->name,
      $record->url,
    );
  }

  $form['projects']['tablesort_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  $form['projects']['note'] = array(
    '#markup' => 'For now, to add/remove projects you need to edit the code_qa_project table.',
  );
}
