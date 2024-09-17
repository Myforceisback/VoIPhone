#include "MainForm.h"
#include <locale>
using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<String^>^ args) {
	setlocale(LC_ALL, "Rus");
	Application::SetCompatibleTextRenderingDefault(false);
	Application::EnableVisualStyles();
	VoIPhone::MainForm mainForm;
	Application::Run(% mainForm); //Запуск главного окна
}
