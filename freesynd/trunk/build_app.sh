#!/bin/bash

# set version
ver=0.5

# build .app layout
app="FreeSynd.app"
mkdir -p "$app/Contents/Framework"
mkdir -p "$app/Contents/Resources"
mkdir -p "$app/Contents/MacOS"

# write Info.plist
cat > "$app/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleDevelopmentRegion</key>
	<string>English</string>
	<key>CFBundleName</key>
	<string>FreeSynd</string>
	<key>CFBundleExecutable</key>
	<string>freesynd</string>
	<key>CFBundleIdentifier</key>
	<string>com.freesynd.FreeSynd</string>
	<key>CFBundleShortVersionString</key>
	<string>$ver</string>
	<key>CFBundleIconFile</key>
	<string>sword.icns</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleGetInfoString</key>
	<string></string>
	<key>CSResourcesFileMapped</key>
	<true/>
	<key>LSRequiresCarbon</key>
	<true/>
	<key>NSHumanReadableCopyright</key>
	<string>© 2011 FreeSynd Team</string>
</dict>
</plist>
EOF

exe="$app/Contents/MacOS/freesynd"
rsrc="$app/Contents/Resources"
fw="$app/Contents/Framework"

# Install executable
cp src/freesynd "$exe"

# Install icon
cp icon/sword.icns "$rsrc/sword.icns"

# Install data
cp -R data "$rsrc/our_data"

# Install freesynd.ini
cp freesynd.ini "$rsrc/freesynd.ini"

local_deps_of()
{
	otool -L "$1" | cut -f 2 | grep ^/usr/local | cut -f 1 -d ' '
}

old_to_new()
{
	echo "$1" | sed -e 's|/usr/local/lib|@executable_path/../Framework|'
}

list=()
while read dep; do
	echo "Found dep $dep"
	list[${#list[@]}]=$dep
done < <(local_deps_of "$exe")

# FIXME: do we need re-iterate over the collected libs now and get their deps?  or would that show up under the exe's deps?

for (( i = 0; i < ${#list[@]}; i++ )); do
	old="${list[$i]}"
	new=`old_to_new "$old"`
	base=`basename "$old"`
	echo "Installing $base to Framework"
	cp "$old" "$fw/"
	echo "Changing id of $fw/$base to $new"
	install_name_tool -id "$new" "$fw/$base"
done

list[${#list[@]}]=$exe
for (( i = 0; i < ${#list[@]}; i++ )); do
	cur="${list[$i]}"
	if echo "$cur" | grep '^/usr/local'; then
		cur=`basename "$cur"`
		cur="$fw/$cur"
	fi
	echo "Checking deps of $cur"
	local_deps_of "$cur" | while read dep; do
		new=`old_to_new "$dep"`
		echo "Modifying deps of $cur from $dep to $new"
		install_name_tool -change "$dep" "$new" "$cur"
	done
done

