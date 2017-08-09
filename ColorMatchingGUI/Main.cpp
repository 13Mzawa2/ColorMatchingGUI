# include <Siv3DAddon/OpenCV.hpp>
# include <Siv3D.hpp>

//	global variables
std::array<cv::Vec3d, 4> A_calib, B_calib;	//	L, gamma, beta, offset(b,g,r)
std::array<cv::Vec3d, 2> A_xy, B_xy;	//	x(b,g,r), y(b,g,r)
//	フラグ
static bool csv_loaded = false;
static bool img_loaded = false;

void createSettingGUI(GUI gui)
{
	gui.setTitle(L"設定");

	gui.addln(L"csvbutton", GUIButton::Create(L"色校正データ読込"));
	gui.add(L"hr1", GUIHorizontalLine::Create(1));
	gui.horizontalLine(L"hr1").style.color = Color(127);

	//	画像を表示していたディスプレイの特性
	gui.addln(GUIText::Create(L"読込画像のディスプレイ(A)の色特性"));
	gui.addln(GUIText::Create(L"x\ty\tL\tγ\tβ"));
	gui.add(GUIText::Create(L"B"));
	gui.add(L"Axb", GUITextField::Create(4));
	gui.add(L"Ayb", GUITextField::Create(4));
	gui.add(L"ALb", GUITextField::Create(4));
	gui.add(L"Agammab", GUITextField::Create(4));
	gui.add(L"Abetab", GUITextField::Create(4));
	gui.addln(L"Aoffsetb", GUITextField::Create(4));
	gui.add(GUIText::Create(L"G"));
	gui.add(L"Axg", GUITextField::Create(4));
	gui.add(L"Ayg", GUITextField::Create(4));
	gui.add(L"ALg", GUITextField::Create(4));
	gui.add(L"Agammag", GUITextField::Create(4));
	gui.add(L"Abetag", GUITextField::Create(4));
	gui.addln(L"Aoffsetg", GUITextField::Create(4));
	gui.add(GUIText::Create(L"R"));
	gui.add(L"Axr", GUITextField::Create(4));
	gui.add(L"Ayr", GUITextField::Create(4));
	gui.add(L"ALr", GUITextField::Create(4));
	gui.add(L"Agammar", GUITextField::Create(4));
	gui.add(L"Abetar", GUITextField::Create(4));
	gui.addln(L"Aoffsetr", GUITextField::Create(4));

	//	画像を表示したいディスプレイの特性
	gui.addln(GUIText::Create(L"画像を表示したいディスプレイ(B)の色特性"));
	gui.addln(GUIText::Create(L"x\ty\tL\tγ\tβ"));
	gui.add(GUIText::Create(L"B"));
	gui.add(L"Bxb", GUITextField::Create(4));
	gui.add(L"Byb", GUITextField::Create(4));
	gui.add(L"BLb", GUITextField::Create(4));
	gui.add(L"Bgammab", GUITextField::Create(4));
	gui.add(L"Bbetab", GUITextField::Create(4));
	gui.addln(L"Boffsetb", GUITextField::Create(4));
	gui.add(GUIText::Create(L"G"));
	gui.add(L"Bxg", GUITextField::Create(4));
	gui.add(L"Byg", GUITextField::Create(4));
	gui.add(L"BLg", GUITextField::Create(4));
	gui.add(L"Bgammag", GUITextField::Create(4));
	gui.add(L"Bbetag", GUITextField::Create(4));
	gui.addln(L"Boffsetg", GUITextField::Create(4));
	gui.add(GUIText::Create(L"R"));
	gui.add(L"Bxr", GUITextField::Create(4));
	gui.add(L"Byr", GUITextField::Create(4));
	gui.add(L"BLr", GUITextField::Create(4));
	gui.add(L"Bgammar", GUITextField::Create(4));
	gui.add(L"Bbetar", GUITextField::Create(4));
	gui.addln(L"Boffsetr", GUITextField::Create(4));

	//	画像読み込み&変換&保存
	gui.add(L"hr1", GUIHorizontalLine::Create(1));
	gui.horizontalLine(L"hr1").style.color = Color(127);
	gui.addln(L"imgbutton", GUIButton::Create(L"画像読み込み"));
	gui.add(L"cvtbutton", GUIButton::Create(L"画像変換", false));
	gui.addln(L"savebutton", GUIButton::Create(L"変換画像の保存", false));
}

void readCSV(GUI gui)
{
	if (const auto open = Dialog::GetOpen({ { L"CSVファイル (*.csv)", L"*.csv" } }))
	{
		const CSVReader csv(open.value());
		if (!csv)return;
		for (int i = 0; i < 3; i++) {
			A_xy[0][i] = csv.get<double>(3 - i, 1); A_xy[1][i] = csv.get<double>(3 - i, 2);
			B_xy[0][i] = csv.get<double>(7 - i, 1); B_xy[1][i] = csv.get<double>(7 - i, 2);
			for (int j = 0; j < 4; j++) {
				A_calib[j][i] = csv.get<double>(3 - i, j+3);
				B_calib[j][i] = csv.get<double>(7 - i, j+3);
			}
		}
		//	値の書き込み
		gui.textField(L"Axb").setText(Format(DecimalPlace(3), A_xy[0][0]));
		gui.textField(L"Axg").setText(Format(DecimalPlace(3), A_xy[0][1]));
		gui.textField(L"Axr").setText(Format(DecimalPlace(3), A_xy[0][2]));
		gui.textField(L"Ayb").setText(Format(DecimalPlace(3), A_xy[1][0]));
		gui.textField(L"Ayg").setText(Format(DecimalPlace(3), A_xy[1][1]));
		gui.textField(L"Ayr").setText(Format(DecimalPlace(3), A_xy[1][2]));
		gui.textField(L"ALb").setText(Format(DecimalPlace(3), A_calib[0][0]));
		gui.textField(L"ALg").setText(Format(DecimalPlace(3), A_calib[0][1]));
		gui.textField(L"ALr").setText(Format(DecimalPlace(3), A_calib[0][2]));
		gui.textField(L"Agammab").setText(Format(DecimalPlace(3), A_calib[1][0]));
		gui.textField(L"Agammag").setText(Format(DecimalPlace(3), A_calib[1][1]));
		gui.textField(L"Agammar").setText(Format(DecimalPlace(3), A_calib[1][2]));
		gui.textField(L"Abetab").setText(Format(DecimalPlace(3), A_calib[2][0]));
		gui.textField(L"Abetag").setText(Format(DecimalPlace(3), A_calib[2][1]));
		gui.textField(L"Abetar").setText(Format(DecimalPlace(3), A_calib[2][2]));
		gui.textField(L"Aoffsetb").setText(Format(DecimalPlace(3), A_calib[3][0]));
		gui.textField(L"Aoffsetg").setText(Format(DecimalPlace(3), A_calib[3][1]));
		gui.textField(L"Aoffsetr").setText(Format(DecimalPlace(3), A_calib[3][2]));
		
		gui.textField(L"Bxb").setText(Format(DecimalPlace(3), B_xy[0][0]));
		gui.textField(L"Bxg").setText(Format(DecimalPlace(3), B_xy[0][1]));
		gui.textField(L"Bxr").setText(Format(DecimalPlace(3), B_xy[0][2]));
		gui.textField(L"Byb").setText(Format(DecimalPlace(3), B_xy[1][0]));
		gui.textField(L"Byg").setText(Format(DecimalPlace(3), B_xy[1][1]));
		gui.textField(L"Byr").setText(Format(DecimalPlace(3), B_xy[1][2]));
		gui.textField(L"BLb").setText(Format(DecimalPlace(3), A_calib[0][0]));
		gui.textField(L"BLg").setText(Format(DecimalPlace(3), A_calib[0][1]));
		gui.textField(L"BLr").setText(Format(DecimalPlace(3), A_calib[0][2]));
		gui.textField(L"Bgammab").setText(Format(DecimalPlace(3), A_calib[1][0]));
		gui.textField(L"Bgammag").setText(Format(DecimalPlace(3), A_calib[1][1]));
		gui.textField(L"Bgammar").setText(Format(DecimalPlace(3), A_calib[1][2]));
		gui.textField(L"Bbetab").setText(Format(DecimalPlace(3), A_calib[2][0]));
		gui.textField(L"Bbetag").setText(Format(DecimalPlace(3), A_calib[2][1]));
		gui.textField(L"Bbetar").setText(Format(DecimalPlace(3), A_calib[2][2]));
		gui.textField(L"Boffsetb").setText(Format(DecimalPlace(3), B_calib[3][0]));
		gui.textField(L"Boffsetg").setText(Format(DecimalPlace(3), B_calib[3][1]));
		gui.textField(L"Boffsetr").setText(Format(DecimalPlace(3), B_calib[3][2]));

		csv_loaded = true;

		//	変換ボタンを使用可能にする
		if (img_loaded) {
			gui.button(L"cvtbutton").enabled = true;
		}
	}
}

void cvtDisplay(cv::Mat src_, cv::Mat &dst_,
	std::array<cv::Vec3b, 4>calib_a, std::array<cv::Vec3b, 2>xy_a,
	std::array<cv::Vec3b, 4>calib_b, std::array<cv::Vec3b, 2>xy_b) 
{
	//	変数の展開
	cv::Vec3b L_a = calib_a[0];
	cv::Vec3b gamma_a = calib_a[1];
	cv::Vec3b beta_a = calib_a[2];
	cv::Vec3b offset_a = calib_a[3];
	cv::Vec3b L_b = calib_b[0];
	cv::Vec3b gamma_b = calib_b[1];
	cv::Vec3b beta_b = calib_b[2];
	cv::Vec3b offset_b = calib_b[3];

	//	xy色度図から変換行列を作成


	//	入力画像をXYZ色空間上に変換

}

void Main()
{
	Window::Resize(1000, 800);
	Window::SetPos(100, 100);
	Window::SetStyle(WindowStyle::Sizeable);

	GUI setting_gui(GUIStyle::Default);
	createSettingGUI(setting_gui);

	//	画像格納場所
	cv::Mat srcImg, dstImg;
	//	画像表示用
	DynamicTexture tex;

	while (System::Update())
	{
		//	Event
		if (setting_gui.button(L"csvbutton").pushed) {
			readCSV(setting_gui);
		}
		if (setting_gui.button(L"imgbutton").pushed) {
			if (const auto open = Dialog::GetOpenImage()) {
				Image src(open.value());
				tex.tryFill(src);
				//	Siv3D -> OpenCV
				srcImg = OpenCV::ToMatVec3b(src);
				cv::cvtColor(srcImg, srcImg, cv::COLOR_BGR2RGB);

				//	フラグ処理
				img_loaded = true;
				if (csv_loaded) {
					setting_gui.button(L"cvtbutton").enabled = true;
				}
			}
		}
		if (setting_gui.button(L"cvtbutton").pushed) {
			
		}

		//	Update
		if (!tex.isEmpty()) {
			tex.draw();
		}
	}
}
