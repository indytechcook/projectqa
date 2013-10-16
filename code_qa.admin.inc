<?php

/**
 * @file Admin pages and functions
 */

/**
 * Callback for module page
 */
function code_qa_admin_page($form, &$form_state) {
  $form = array();

  $form['phploc'] = array(
    '#type' => 'fieldset',
    '#title' => t('PHPLOC'),
    '#tree' => TRUE,
  );

  $phploc_actions = array();

  $phploc_actions['run'] = array(
    '#type' => 'submit',
    '#value' => t('Run PHPLOC'),
    '#submit' => array('code_qa_run_phploc'),
  );
  $phploc_actions['view'] = array(
    '#type' => 'submit',
    '#value' => t('View results'),
    '#submit' => array('code_qa_view_phploc'),
  );

  $form['phploc']['action'] = $phploc_actions;


  return $form;
}

