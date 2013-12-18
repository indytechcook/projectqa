<?php

/**
 * Helper function to execute git commands against a repo.
 *
 * @param string $repo_path
 *   The system path where the repo is located.
 *
 * @param string $command
 *   The git command to run.
 *
 * @param array $parameters
 *   An array of parameters to pass to the git command.
 *
 * @return array
 *   The output from the command.
 */
function project_qa_execute_git($repo_path, $command, $parameters = array()) {

  $full_command = 'git --git-dir=' . $repo_path
    . '/.git --work-tree=' . $repo_path
    . ' ' . $command . ' ' . implode(' ', $parameters);

  exec($full_command, $output);

  return $output;
}


function project_qa_get_parent_hash($repo_path, $hash) {

  // http://paulboxley.com/blog/2011/06/git-caret-and-tilde

  $command = 'log';
  $parameters = array(
    '--pretty="format:%H"',
    '-n 1',
    $hash . '~',
  );
  $parent = project_qa_execute_git($repo_path, $command, $parameters);

  return $parent;
}


function project_qa_get_all_tags($repo_path) {

  // git show-ref --tags -d

  $command = 'show-ref';
  $parameters = array(
    '--tags',
    '-d',
  );
  $tags = project_qa_execute_git($repo_path, $command, $parameters);

  return $tags;
}
