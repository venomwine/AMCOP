# AMCOP

Android NDK(Native Activity)/MFC Cross-platform OpenGL Project (AMCOP).
The same engine can operate in Android(NDK) and Windows(MFC).
Forget the difficult and complex ndk debugger. Use powerful visual studio debugger.

## Import

### Android
- Import `Existing Android Code Into Workspace.`
- Check your android-ndk path. > `Properties` > `Builders` > `AMCOP_NDK` > `Main` tab > `Location`

### MFC
- Open Project file
- /AMCOP/vsProject/ **AMCOP.sln**

## Important

- If you add a file to a visual studio project, You don't forget android.mk file check.
- If an error occurs in the OpenGL, Take a look at the "/AMCOP/vsProject/lib_opengl" folder. 

## Author

- Wooram, Hong-
- https://github.com/venomwine/AMCOP
- https://play.google.com/store/apps/details?id=com.amcop.app
- venomwine@gmail.com 
 
## License

Licensed under [GPLv3][gplv3]. Enjoy.

[gplv3]: http://opensource.org/licenses/GPL-3.0
 