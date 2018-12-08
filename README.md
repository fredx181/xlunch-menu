# Xlunch-menu      
(for testing, can possibly improved)         

Xlunch-menu can be used to generate entries file (and is very fast :) ) for xlunch:    
https://github.com/Tomas-M/xlunch  
Most of the work is done by the LXDE library menu-cache.  

This is modified "openbox-menu" from here:  
https://bitbucket.org/geekless/openbox-menu/  

### Compilation.  

Type `make` then, with superuser privileges, `make install` to install it.  
xlunch-menu is installed in /usr/local/bin directory by default, type    
`make install DESTDIR=/usr` to install it to /usr/bin/directory.

To test (will create ./entries.dsv:     
`make check`   

### Dependencies:    
For compiling:  
menu-cache , gtk2 (for Debian: libmenu-cache-dev, libgtk2.0-dev)    

### At runtime (optional, for converting SVG to PNG)  
librsvg (for Debian, package: librsvg2-bin) for providing `rsvg-convert`

Type: `xlunch-menu --help` for usage    

### Examples usage:
For a one shot creating or updating $HOME/.config/xlunch/entries.dsv:  
```
mkdir -p $HOME/.config/xlunch/ 2> /dev/null        
xlunch-menu /path/to/xlunch-applications.menu -I 128 -o $HOME/.config/xlunch/entries.dsv       
```
Running script: `xlunch-menu-update` does this, and also converts svg to png (if `rsvg-convert` installed)     
   
***Or:*** persistent (-p) mode (running in the background, entries.dsv will be updated if new .desktop launcher has been added to /usr/share/applications or ~/.local/share/applications):           
    
```        
pkill xlunch-menu   # prevent multiple processes of xlunch-menu  
mkdir -p $HOME/.config/xlunch/ 2> /dev/null   
entriesfile=$HOME/.config/xlunch/entries.dsv          
xlunch-menu xlunch-applications.menu -I 128 -p -o $entriesfile 2> /dev/null &   
disown     
     
```        
Running script: `xlunch-menu-gen` does similar, and also converts svg to png (if `rsvg-convert` installed)   
Add xlunch-menu-gen to Startup applications and the entries.dsv file will be automatically updated when installing or removing an application.

### Additional info:   
If you do have `rsvg-convert` installed, I'd recommend to edit the Makefile and uncomment "# CFLAGS+=-DWITH_SVG", so becomes:       
`CFLAGS+=-DWITH_SVG`   
(see more info in Makefile)     
Then running `xlunch-menu-gen` or `xlunch-menu-update` gives better chance that the most icons will display in xlunch.      

       


