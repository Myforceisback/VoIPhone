#pragma once

namespace VoIPhone {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// —водка дл€ IncomigCallForm
	/// </summary>
	public ref class IncomigCallForm : public System::Windows::Forms::Form
	{
	public:
		IncomigCallForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}
		IncomigCallForm(System::String^ callInfo)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
			this->userCallLabel->Text = callInfo;
		}

	protected:
		/// <summary>
		/// ќсвободить все используемые ресурсы.
		/// </summary>
		~IncomigCallForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^ userCallLabel;
	private: System::Windows::Forms::Button^ acceptCallButton;
	private: System::Windows::Forms::Button^ denyCallButton;
	protected:

	protected:

	private:
		/// <summary>
		/// ќб€зательна€ переменна€ конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;
	private:
		bool state;
		System::String^ info;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// “ребуемый метод дл€ поддержки конструктора Ч не измен€йте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->userCallLabel = (gcnew System::Windows::Forms::Label());
			this->acceptCallButton = (gcnew System::Windows::Forms::Button());
			this->denyCallButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// userCallLabel
			// 
			this->userCallLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->userCallLabel->Font = (gcnew System::Drawing::Font(L"Arial", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->userCallLabel->Location = System::Drawing::Point(12, 9);
			this->userCallLabel->Name = L"userCallLabel";
			this->userCallLabel->Size = System::Drawing::Size(260, 87);
			this->userCallLabel->TabIndex = 0;
			this->userCallLabel->Text = L"label1";
			this->userCallLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// acceptCallButton
			// 
			this->acceptCallButton->Location = System::Drawing::Point(12, 117);
			this->acceptCallButton->Name = L"acceptCallButton";
			this->acceptCallButton->Size = System::Drawing::Size(120, 40);
			this->acceptCallButton->TabIndex = 1;
			this->acceptCallButton->Text = L"ќтветить";
			this->acceptCallButton->UseVisualStyleBackColor = true;
			this->acceptCallButton->Click += gcnew System::EventHandler(this, &IncomigCallForm::acceptCallButton_Click);
			// 
			// denyCallButton
			// 
			this->denyCallButton->Location = System::Drawing::Point(152, 117);
			this->denyCallButton->Name = L"denyCallButton";
			this->denyCallButton->Size = System::Drawing::Size(120, 40);
			this->denyCallButton->TabIndex = 2;
			this->denyCallButton->Text = L"ќтклонить";
			this->denyCallButton->UseVisualStyleBackColor = true;
			this->denyCallButton->Click += gcnew System::EventHandler(this, &IncomigCallForm::denyCallButton_Click);
			// 
			// IncomigCallForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->ClientSize = System::Drawing::Size(284, 177);
			this->Controls->Add(this->denyCallButton);
			this->Controls->Add(this->acceptCallButton);
			this->Controls->Add(this->userCallLabel);
			this->Cursor = System::Windows::Forms::Cursors::Hand;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"IncomigCallForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"¬ход€щий вызов";
			this->ResumeLayout(false);

		}
#pragma endregion

	public:
		bool getState() {
			return state;
		}
	private:
		System::Void acceptCallButton_Click(System::Object^ sender, System::EventArgs^ e) {
			this->state = true;
			Close();
		}
 
		System::Void denyCallButton_Click(System::Object^ sender, System::EventArgs^ e) {
			this->state = false;
			Close();
		}
	};
}
