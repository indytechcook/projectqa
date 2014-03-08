<?php

class ProjectQAWebHookUnserialize implements  Webhook_Plugins_Unserializer_Interface {
  /**
   * Unserialize POST data into stdClass object.
   *
   * @param string $data
   *   The data sent via POST
   *
   * @return stdClass
   *   The received JSON data as an stdClass object.
   */
  public function unserialize($data) {
    /*
     * Make sure that this method returns an stdClass object so the processor
     * class can handle it properly.
     */
    $post_data = new stdClass();

    $url = webhook_load_unserializer('urlencoded');
    $vars = $url->unserialize($data);
    $json = webhook_load_unserializer('json');
    $secret_key = $this->getSecretKey();

    $hash = md5($vars['data'] . $secret_key);

    if ($vars['hash'] == $hash) {
      if (isset($vars['data'])) {
        $post_data = $json->unserialize($vars['data']);
      }

      return $post_data;
    }
  }

  /**
   * Get secret key form database.
   * @return string
   *   The secret key
   */
  protected function getSecretKey() {
    return variable_get('projectqa_webhook_plugin_secret_key', '');
  }
}