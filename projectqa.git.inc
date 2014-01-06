<?php
/**
 * @file
 * Git related functions.
 */

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
function projectqa_execute_git($repo_path, $command, $parameters = array()) {

  $full_command = 'git --git-dir=' . $repo_path
    . '/.git --work-tree=' . $repo_path
    . ' ' . $command . ' ' . implode(' ', $parameters);

  exec($full_command, $output);

  return $output;
}

/**
 * Retrieves the hash of the parent commit.
 *
 * @param string $repo_path
 *   The system path where the repo is located.
 *
 * @param string $hash
 *   The hash of the commit to find the parent for.
 *
 * @return string
 *   The hash of the parent commit.
 */
function projectqa_get_parent_hash($repo_path, $hash) {

  // http://paulboxley.com/blog/2011/06/git-caret-and-tilde
  $command = 'log';
  $parameters = array(
    '--pretty="format:%H"',
    '-n 1',
    $hash . '~',
  );
  $parent = projectqa_execute_git($repo_path, $command, $parameters);

  return $parent;
}


/**
 * Retrieves all the git tags for a repo.
 *
 * @param string $repo_path
 *   The system path where the repo is located.
 *
 * @return array
 *   An array of tags in the repo.
 */
function projectqa_get_all_tags($repo_path) {

  // Git show-ref --tags -d
  $command = 'show-ref';
  $parameters = array(
    '--tags',
    '-d',
  );
  $tags = projectqa_execute_git($repo_path, $command, $parameters);

  return $tags;
}
