<?php
/**
 * @file
 * phploc_views.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function phploc_views_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'project_complexity';
  $view->description = 'Lists projects and their complexity values';
  $view->tag = 'project_qa';
  $view->base_table = 'phploc_result';
  $view->human_name = 'Project complexity';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Project complexity';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_tags'] = array(
    0 => 'project_qa_complexity',
  );
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'name' => 'name',
    'ccnByLloc' => 'ccnByLloc',
    'ccnByNom' => 'ccnByNom',
    'timestamp' => 'timestamp',
  );
  $handler->display->display_options['style_options']['default'] = 'ccnByLloc';
  $handler->display->display_options['style_options']['info'] = array(
    'name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'ccnByLloc' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'ccnByNom' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'timestamp' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: PHPLOC Result: Git commit ID */
  $handler->display->display_options['relationships']['gcid']['id'] = 'gcid';
  $handler->display->display_options['relationships']['gcid']['table'] = 'phploc_result';
  $handler->display->display_options['relationships']['gcid']['field'] = 'gcid';
  $handler->display->display_options['relationships']['gcid']['required'] = TRUE;
  /* Relationship: Project QA Git Commit: Project ID */
  $handler->display->display_options['relationships']['pid']['id'] = 'pid';
  $handler->display->display_options['relationships']['pid']['table'] = 'project_qa_gitcommit';
  $handler->display->display_options['relationships']['pid']['field'] = 'pid';
  $handler->display->display_options['relationships']['pid']['relationship'] = 'gcid';
  $handler->display->display_options['relationships']['pid']['required'] = TRUE;
  /* Field: Project QA Project: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'project_qa_repo';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'pid';
  $handler->display->display_options['fields']['name']['label'] = 'Project';
  /* Field: PHPLOC Result: ccnByLloc */
  $handler->display->display_options['fields']['ccnByLloc']['id'] = 'ccnByLloc';
  $handler->display->display_options['fields']['ccnByLloc']['table'] = 'phploc_result';
  $handler->display->display_options['fields']['ccnByLloc']['field'] = 'ccnByLloc';
  $handler->display->display_options['fields']['ccnByLloc']['label'] = 'By LLOC';
  $handler->display->display_options['fields']['ccnByLloc']['precision'] = '0';
  /* Field: PHPLOC Result: ccnByNom */
  $handler->display->display_options['fields']['ccnByNom']['id'] = 'ccnByNom';
  $handler->display->display_options['fields']['ccnByNom']['table'] = 'phploc_result';
  $handler->display->display_options['fields']['ccnByNom']['field'] = 'ccnByNom';
  $handler->display->display_options['fields']['ccnByNom']['label'] = 'By # of methods';
  $handler->display->display_options['fields']['ccnByNom']['precision'] = '0';
  /* Field: Project QA Git Commit: Timestamp */
  $handler->display->display_options['fields']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['table'] = 'project_qa_gitcommit';
  $handler->display->display_options['fields']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['relationship'] = 'gcid';
  $handler->display->display_options['fields']['timestamp']['label'] = 'Last recorded commit';
  $handler->display->display_options['fields']['timestamp']['date_format'] = 'long';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'project-complexity';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Project complexity';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'main-menu';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $export['project_complexity'] = $view;

  $view = new view();
  $view->name = 'top_contributors';
  $view->description = 'Lists the commit authors across all projects ordered by lines of code delta';
  $view->tag = 'project_qa';
  $view->base_table = 'phploc_result_delta';
  $view->human_name = 'Top contributors';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Top contributors';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['group_by'] = TRUE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'author' => 'author',
    'lloc_delta' => 'lloc_delta',
    'loc_delta' => 'loc_delta',
    'cloc_delta' => 'cloc_delta',
  );
  $handler->display->display_options['style_options']['default'] = 'lloc_delta';
  $handler->display->display_options['style_options']['info'] = array(
    'author' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'lloc_delta' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'loc_delta' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'cloc_delta' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: PHPLOC Result Delta: Git commit ID */
  $handler->display->display_options['relationships']['gcid']['id'] = 'gcid';
  $handler->display->display_options['relationships']['gcid']['table'] = 'phploc_result_delta';
  $handler->display->display_options['relationships']['gcid']['field'] = 'gcid';
  /* Field: Project QA Git Commit: Author */
  $handler->display->display_options['fields']['author']['id'] = 'author';
  $handler->display->display_options['fields']['author']['table'] = 'project_qa_gitcommit';
  $handler->display->display_options['fields']['author']['field'] = 'author';
  $handler->display->display_options['fields']['author']['relationship'] = 'gcid';
  /* Field: SUM(PHPLOC Result Delta: Logical lines of code (LLOC) (delta)) */
  $handler->display->display_options['fields']['lloc_delta']['id'] = 'lloc_delta';
  $handler->display->display_options['fields']['lloc_delta']['table'] = 'phploc_result_delta';
  $handler->display->display_options['fields']['lloc_delta']['field'] = 'lloc_delta';
  $handler->display->display_options['fields']['lloc_delta']['group_type'] = 'sum';
  /* Field: SUM(PHPLOC Result Delta: Lines of Code (delta)) */
  $handler->display->display_options['fields']['loc_delta']['id'] = 'loc_delta';
  $handler->display->display_options['fields']['loc_delta']['table'] = 'phploc_result_delta';
  $handler->display->display_options['fields']['loc_delta']['field'] = 'loc_delta';
  $handler->display->display_options['fields']['loc_delta']['group_type'] = 'sum';
  /* Field: SUM(PHPLOC Result Delta: Commented lines of code (delta)) */
  $handler->display->display_options['fields']['cloc_delta']['id'] = 'cloc_delta';
  $handler->display->display_options['fields']['cloc_delta']['table'] = 'phploc_result_delta';
  $handler->display->display_options['fields']['cloc_delta']['field'] = 'cloc_delta';
  $handler->display->display_options['fields']['cloc_delta']['group_type'] = 'sum';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'top-contributors';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Top contributors';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'main-menu';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $export['top_contributors'] = $view;

  return $export;
}
