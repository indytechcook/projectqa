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

  $phploc_action = array();
  if(code_qa_detect_phploc()) {
    $phploc_status = t('found');
    $phploc_action = array(
      '#type' => 'submit',
      '#value' => t('Run PHPLOC'),
      '#submit' => array('code_qa_run_phploc'),
    );
  }
  else {
    $phploc_status = t('MISSING');
    // $phploc_action = array(
    //   '#type' => 'submit',
    //   '#value' => t('Install PHPLOC'),
    //   '#submit' => array('code_qa_download_phploc'),
    // );
  }

  $form['phploc']['intro'] = array(
    '#type' => 'markup',
    '#markup' => t('Detecting PHPLOC ... @status', array('@status' => $phploc_status)),
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );

  $form['phploc']['action'] = $phploc_action;


  return $form;
}

