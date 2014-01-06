# ProjectQA

Tools to monitor the code of your Drupal site. Inspired by this Drupalcon Portland session: https://portland2013.drupal.org/node/1078

## Setup

1. Download this module to a Drupal site.
2. Use drush to install this module and submodule: `drush en projectqa projectqa_phploc -y` (using the UI will not handle the composer dependencies).
2. Add a repo to track to the projectqa_repo table.
3. Run `drush pqa` and grab some popcorn or a beer (especially if the project is a big one).
4. Use views to create reports on the data. Two default views for PHPLOC are included.

*There are sample views included as well, but they are not enabled by default. You need to enable the projectqa_phploc_views module to see them.