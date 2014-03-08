<?php


class ProjectQAWebHookProcessor implements Webhook_Plugins_Processor_Interface {
  const ALL_PROJECTS = 'all';

  /**
   * Configuration form.
   *
   * Provides a configuration form for the plugin.
   *
   * This is reserved for future use, but isn't currently implemented.
   *
   * @return array
   *   A Form API form structure array
   */
  public function config_form() {
    return array();
  }

  /**
   * Processes data.
   *
   * @param stdClass $data
   *  The encoded data
   *
   * @return object
   *   The unserialized data as a PHP object.
   *
   * @throws Webhook_Plugins_Processor_ErrorException
   */
  public function process(stdClass $data) {
    $status = 'success';
    $message = 'Success.';
    $process_result = '{
        "status": "%status",
        "message": "%message"
      }';

    $required_attributes = $this->getRequiredAttributes();

    foreach ($required_attributes as $attribute) {
      // Make sure that all properties are properly set.
      if (!property_exists($data, $attribute) || $data->$attribute === '') {
        throw new Webhook_Plugins_Processor_ErrorException('Missing data information', 400);
      }
    }

    if ($status != 'error') {
      if ($data->project_id == self::ALL_PROJECTS) {
        projectqa_walk_projects();
        return $this->returnMessage($status, $message, $process_result);
      } else {
        $repo = projectqa_repo_load($data->project_id);

        if ($repo) {
          projectqa_process_project($repo);
          return $this->returnMessage($status, $message, $process_result);
        }
        else {
          throw new Webhook_Plugins_Processor_ErrorException('Invalid Repo ID');
        }
      }
    }
  }

  protected function returnMessage($status, $message, $process_result) {
    return str_replace(
      array("%status", "%message"),
      array($status, $message),
      $process_result
    );
  }

  /**
   * Get all required attributes for the processor to work properly.
   *
   * @return array
   *   List of all the required attributes for the processor to work properly.
   */
  protected function getRequiredAttributes() {
    return array(
      'project_id',
    );
  }
}