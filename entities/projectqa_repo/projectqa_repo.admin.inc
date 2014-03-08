<?php

/**
 * @file
 * Repo editing UI.
 */

/**
 * UI controller.
 */
class ProjectQAUIController extends EntityDefaultUIController {

}

/**
 * Form callback: create or edit a repo.
 *
 * @param $repo
 *   The repo object to edit or for a create form an empty repo object
 *     with only a model type defined.
 */
function projectqa_repo_form($form, &$form_state, $repo) {
  // Add the default field elements.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Repo Name'),
    '#default_value' => isset($repo->name) ? $repo->name : '',
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );

  $form['url'] = array(
    '#type' => 'textfield',
    '#title' => t('Repo URL'),
    '#default_value' => isset($repo->url) ? $repo->url : '',
    '#maxlength' => 255,
    '#required' => TRUE,
  );

  $form['branch'] = array(
    '#type' => 'textfield',
    '#title' => t('Branch Name'),
    '#default_value' => isset($repo->branch) ? $repo->branch : '',
    '#maxlength' => 255,
    '#required' => TRUE,
  );

  if (module_exists("projectqa_phploc")) {
    $form["phploc"] = array(
      "#type" => "checkbox",
      "#title" => t("Enable PHPLOC Processing"),
      '#default_value' => isset($repo->phploc) ? $repo->phploc : true,
    );
  }

  // Add the field related form elements.
  $form_state['projectqa_repo'] = $repo;
  field_attach_form('projectqa_repo', $repo, $form, $form_state);

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save repo'),
    '#submit' => $submit + array('projectqa_repo_edit_form_submit'),
  );

  if (!empty($repo->name)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete repo'),
      '#suffix' => l(t('Cancel'), 'admin/content/projectqa-repos'),
      '#submit' => $submit + array('projectqa_repo_form_submit_delete'),
      '#weight' => 45,
    );
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'projectqa_repo_edit_form_validate';
  return $form;
}


/**
 * Form API validate callback for the repo form
 */
function projectqa_repo_edit_form_validate(&$form, &$form_state) {
  $repo = $form_state['projectqa_repo'];

  // Notify field widgets to validate their data.
  field_attach_form_validate('projectqa_repo', $repo, $form, $form_state);
}


/**
 * Form API submit callback for the repo form.
 *
 * @todo remove hard-coded link
 */
function projectqa_repo_edit_form_submit(&$form, &$form_state) {

  $repo = entity_ui_controller('projectqa_repo')->entityFormSubmitBuildEntity($form, $form_state);
  // Save the model and go back to the list of models

  // Add in created and changed times.
  if ($repo->is_new = isset($repo->is_new) ? $repo->is_new : 0){
    $repo->created = time();
  }

  $repo->changed = time();

  $repo->save();
  $form_state['redirect'] = 'admin/content/projectqa-repos';
}

/**
 * Form API submit callback for the delete button.
 *
 * @todo Remove hard-coded path
 */
function projectqa_repo_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/content/projectqa-repos/manage/' . $form_state['projectqa_repo']->pid . '/delete';
}


/**
 * Form callback: confirmation form for deleting a repo.
 *
 * @param $repo
 *   The repo to delete
 *
 * @see confirm_form()
 */
function projectqa_repo_delete_form($form, &$form_state, $repo) {
  $form_state['projectqa_repo'] = $repo;

  $form['#submit'][] = 'model_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete repo %name?', array('%name' => $repo->name)),
    'admin/content/projectqa-repos/manage',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for projectqa_repo_delete_form
 */
function projectqa_repo_delete_form_submit($form, &$form_state) {
  $repo = $form_state['projectqa_repo'];

  model_delete($repo);

  drupal_set_message(t('The repo %name has been deleted.', array('%name' => $repo->name)));
  watchdog('projectqa_repo', 'Deleted repo %name.', array('%name' => $repo->name));

  $form_state['redirect'] = 'admin/content/projectqa-repos';
}

/**
 * Sets the breadcrumb for administrative repo pages.
 */
function projectqa_repo_set_breadcrumb() {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
      l(t('Administration'), 'admin'),
      l(t('Content'), 'admin/content'),
      l(t('Repos'), 'admin/content/projectqa-repos'),
    );

    drupal_set_breadcrumb($breadcrumb);
  }