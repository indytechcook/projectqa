# Project QA

Tools to monitor the code of your Drupal site. Inspired by this Drupalcon Portland session: https://portland2013.drupal.org/node/1078

## Setup

1. Use drush to install this module: `drush en project_qa -y` (using the UI will not handle the composer dependencies)
2. Add a repo to track to the project_qa_repo table
3. Run `drush pqa`
4. Use views to create reports on the data
