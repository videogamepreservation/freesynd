<?php $title = 'Development'; require_once 'include/common.php'; ?>

<h3>Getting the code</h3>

<p>The FreeSynd CVS repository is hosted by SourceForge. To checkout a module
from the repository simply type the following commands in your shell (press
enter if the login command asks for a password):</p>

<pre>cvs -d:pserver:anonymous@freesynd.cvs.sourceforge.net:/cvsroot/freesynd login 
cvs -z3 -d:pserver:anonymous@freesynd.cvs.sourceforge.net:/cvsroot/freesynd co -P freesynd
</pre>

<br />

<p>The available modules, and what they contain, are as follows:</p>

<pre>freesynd           FreeSynd source code
www                FreeSynd website code (which may or may not be up to date)</pre>

<br />

<p>Web-based CVS access is also available through SourceForge's <a
href="http://freesynd.cvs.sourceforge.net/freesynd">ViewCVS interface</a>. You
might also want to take a look at the <a
href="http://sourceforge.net/projects/freesynd/">project page</a> on
SourceForge.</p>
<p>Please don't bother with SVN, we've yet to put anything useful in it.</p>

<hr />

<h3>CVS commit notifications</h3>
<p>Developers may be interested in the following mailing list; summaries of
all changes made to the CVS repository are posted here.</p>
FreeSynd CVS mailing list<br />
<small>
(<a href="http://lists.sourceforge.net/lists/listinfo/freesynd-cvs">Subscribe/Manage</a> |
<a href="http://sourceforge.net/mailarchive/forum.php?forum=freesynd-cvs">Archive</a>)
</small>

<?php printFooter(); ?>
