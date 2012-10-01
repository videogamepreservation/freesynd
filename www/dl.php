<?php $title = 'Download'; require_once 'include/common.php'; ?>
<?php printHeader(); ?>

<h3>Getting the binaries</h3>

<p>See the releases on <a href="http://sourceforge.net/projects/freesynd/files/">sourceforge</a>.</p>

<p class="warning">PLEASE NOTE: an ORIGINAL copy of Syndicate (not Plus) is
required to use FreeSynd,<br /> we do not redistribute the original data files here.</p>

<hr />

<h3>Getting the code</h3>

<p>The FreeSynd SVN repository is hosted by SourceForge. To checkout a module
from the repository simply type the following commands in your shell:</p>

<pre>svn co svn://svn.code.sf.net/p/freesynd/code/ freesynd 
</pre>

<br />

<p>The available modules, and what they contain, are as follows:</p>

<pre>freesynd                FreeSynd source code
freesynd-experimental   Experimentation around freesynd
libs                    Windows libraries for compiling and running the game
www                     FreeSynd website code (which may or may not be up to date)
scripts	                Tools to help in development
</pre>

<br />

<p>Web-based SVN access is also available through SourceForge's <a
href="http://sourceforge.net/p/freesynd/code/">SVN Web interface</a>. You might also want to take a look at the <a
href="http://sourceforge.net/projects/freesynd/">project page</a> on
SourceForge.</p>

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
