<?php $title = 'Download'; require_once 'include/common.php'; ?>
<?php printHeader(); ?>

<h3>Getting the binaries</h3>

<p>See the releases on <a href="https://sourceforge.net/project/showfiles.php?group_id=93282">sourceforge</a>.</p>

<b>Please note that an original copy of Syndicate is
required to use FreeSynd; we do not redistribute the original data files here,
and we won't make an exception for you - so don't bother asking.</b>

<hr />

<h3>Getting the code</h3>

<p>The FreeSynd SVN repository is hosted by SourceForge. To checkout a module
from the repository simply type the following commands in your shell (press
enter if the login command asks for a password):</p>

<pre>svn co https://freesynd.svn.sourceforge.net/svnroot/freesynd freesynd 
</pre>

<br />

<p>The available modules, and what they contain, are as follows:</p>

<pre>freesynd           FreeSynd source code
freesynd-experimental   Experimentation around freesynd
libs                    Windows libraries for compiling and running the game
www                FreeSynd website code (which may or may not be up to date)
scripts		   Tools to help in development
</pre>

<br />

<p>Web-based SVN access is also available through SourceForge's <a
href="http://freesynd.svn.sourceforge.net/viewvc/freesynd/">ViewSVN interface</a>. You might also want to take a look at the <a
href="http://sourceforge.net/projects/freesynd/">project page</a> on
SourceForge.</p>
<p>Please don't bother with CVS, we don't use it anymore.</p>

<hr />

<h3>SVN commit notifications</h3>
<p>Developers may be interested in the following mailing list; summaries of
all changes made to the SVN repository are posted here.</p>
FreeSynd SVN mailing list<br />
<small>
(<a href="http://lists.sourceforge.net/lists/listinfo/freesynd-cvs">Subscribe/Manage</a> |
<a href="http://sourceforge.net/mailarchive/forum.php?forum=freesynd-cvs">Archive</a>)
</small>

<?php printFooter(); ?>
