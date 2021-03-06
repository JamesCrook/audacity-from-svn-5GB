<?php
/*
 * Copyright 2005 Matt Brubeck
 * This file is licensed under a Creative Commons license:
 * http://creativecommons.org/licenses/by/2.0/
 */
  require_once "main.inc.php";
  $sectionId = "";
  $pageTitle = _("Free Audio Editor and Recorder");
  include "include/header.inc.php";
  include "include/news.inc.php";
  include "latest/versions.inc.php";
  include "include/detect-os.inc.php";

  $download = which_download();
  if ($download == "windows") {
    $download_version = win_exe_version;
    $download_desc = _("for Microsoft Windows");
    $download_page = "windows";
  }
  else if ($download == "mac") {
    $download_version = macosx_version;
    $download_desc = _("for Mac OS 9 or X");
    $download_page = "mac";
  }
  else {
    $download_version = src_version;
    $download_desc = _("For Windows, Mac, or GNU/Linux");
    $download_page = "";
  }
?>
<div id="about">
  <h2><?=_("The Free, Cross-Platform Sound Editor")?></h2>
  <div id="screenshot">
    <!-- TODO: Auto-select or randomly rotate screenshot? -->
    <a title="<?=_("Screenshots")?>" href="about/screenshots"><img alt="<?=_("Screenshots")?>" src="about/images/audacity-linux-small.jpg"></a>
  </div>
  <p><?=_('Audacity is free, open source software for recording and editing sounds.  It is available for Mac OS X, Microsoft Windows, GNU/Linux, and other operating systems.  <a href="about/">Learn more about Audacity...</a>')?></p>
</div>

<div id="download">
  <h3><a href="download/<?=$download_page?>"><?php printf(_("Download Audacity %s"), $download_version)?></a></h3>
  <p><?=$download_desc?></p>
  <?php
    if ($download_page) {
      echo '<p><a href="download/">'._("Other downloads").'</a></p>';
    }
  ?>
</div>

<div id="news">
  <?php
    global $news_items;
    foreach ($news_items as $item) {
      $dateStr = $item->dateStr();
      ?>
      <div class="newsitem">
        <h3><?="$dateStr: $item->title"?></h3>
        <?=$item->body?>
      </div>
      <?php
    }
  ?>
  <h4><a href="about/news"><?=_("More news items...")?></a></h4>
</div>

<form id="notify" method="post" action="http://scripts.dreamhost.com/add_list.cgi">
  <h3><?=_("Get Notified of New Versions")?></h3>
  <p>
  <input type="hidden" name="list" value="audacity-announce">
  <input type="hidden" name="url" value="http://audacity.sourceforge.net/list/subscribed.php">
  <input type="hidden" name="emailconfirmurl" value="http://audacity.sourceforge.net/list/emailconfirm.php">
  <input type="hidden" name="unsuburl" value="http://audacity.sourceforge.net/list/unsubscribed.php">
  <input type="hidden" name="alreadyonurl" value="http://audacity.sourceforge.net/list/alreadyon.php">
  <input type="hidden" name="notonurl" value="http://audacity.sourceforge.net/list/noton.php">
  <input type="hidden" name="invalidurl" value="http://audacity.sourceforge.net/list/invalid.php">
  <input type="hidden" name="domain" value="spaghetticode.org">
  <input type="hidden" name="emailit" value="1">

  <label for="address"><?=_("Email address")?></label>: <input name="address" id="address" class="text">
  <input type="submit" name="submit" value="<?=_("Add")?>">
  <input type="submit" name="unsub" value="<?=_("Remove")?>">
  </p>
</form>
<?php
  include "include/footer.inc.php";
?>
