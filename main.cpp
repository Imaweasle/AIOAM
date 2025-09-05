#include "defs.h"
#include "mainWindow.h"

int main(int argc, char **argv)
{
 QApplication app (argc, argv);

 //Start the main window
 MainWindow window;

 window.show();

 return app.exec();
}
