<?php
// $Id: sitelinks.tpl.php,v 1.8.2.2 2008/04/21 18:36:22 ufku Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $GLOBALS['language']->language; ?>" xml:lang="<?php print $GLOBALS['language']->language; ?>">

<head>
  <title><?php print drupal_get_title(); ?></title>
  <?php print drupal_get_html_head(); ?>
  <?php print drupal_get_css(); ?>
  <?php print drupal_get_js('header'); ?>
  <style media="all" type="text/css">/*Quick-override*/</style>
</head>

<body class="sitelinks">
<div id="sitelinks-messages"><?php print theme('status_messages'); ?></div>
<h1><?php echo drupal_get_title(); ?></h1>
<div class="content">
    <div class="htmlhelppage">
        <?php print $content; ?>
    </div>
</div>

<?php print drupal_get_js('footer'); ?>
</body>

</html>
