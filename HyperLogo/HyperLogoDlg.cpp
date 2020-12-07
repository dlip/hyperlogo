// HyperLogoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HyperLogo.h"
#include "HyperLogoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHyperLogoDlg dialog




CHyperLogoDlg::CHyperLogoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHyperLogoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHyperLogoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMGPREVIEW, m_Preview);
	DDX_Control(pDX, IDC_EDITPREVIEWTXT, editPreviewTxt);
	DDX_Control(pDX, IDC_CHECKAUTOUPDATE, CheckAutoUpdate);
	DDX_Control(pDX, IDC_FONTS, fontCombo);
	DDX_Control(pDX, IDC_EditFontSize, editFontSize);
	DDX_Control(pDX, IDC_EditStrokeWidth, editStrokeWidth);
	DDX_Control(pDX, IDC_EDITXML, EditXMLFile);
	DDX_Control(pDX, IDC_EDITOUTPUT, editOutputDir);
	DDX_Control(pDX, IDC_ProgressOutput, progressOutput);
	DDX_Control(pDX, IDC_CHECKOVERWRITE, CheckOverwrite);
	DDX_Control(pDX, IDC_GenerateLogos, generateButton);
}

BEGIN_MESSAGE_MAP(CHyperLogoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_GenerateLogos, &CHyperLogoDlg::OnBnClickedGenerateLogos)
	ON_BN_CLICKED(IDC_BTNXML, &CHyperLogoDlg::OnBnClickedBtnXML)
	ON_BN_CLICKED(IDC_BTNOUTPUT, &CHyperLogoDlg::OnBnClickedBtnoutput)
	ON_BN_CLICKED(IDC_BTPREVIEW, &CHyperLogoDlg::OnBnClickedBtpreview)
	ON_EN_CHANGE(IDC_EDITPREVIEWTXT, &CHyperLogoDlg::OnEnChangeEditpreviewtxt)
	ON_BN_CLICKED(IDC_CHECKAUTOUPDATE, &CHyperLogoDlg::OnBnClickedCheckAutoUpdate)
	ON_BN_CLICKED(IDC_BtnFill, &CHyperLogoDlg::OnBnClickedBtnFill)
	ON_BN_CLICKED(IDC_BtnStroke, &CHyperLogoDlg::OnBnClickedBtnStroke)
	ON_CBN_SELCHANGE(IDC_FONTS, &CHyperLogoDlg::OnCbnSelChangeFonts)
	//ON_BN_CLICKED(IDR_QUIT, &CHyperLogoDlg::OnBnClickedQuit)
	ON_EN_CHANGE(IDC_EditFontSize, &CHyperLogoDlg::OnEnChangeEditfontsize)
	ON_EN_CHANGE(IDC_EditStrokeWidth, &CHyperLogoDlg::OnEnChangeEditstrokewidth)
	ON_EN_CHANGE(IDC_EDITXML, &CHyperLogoDlg::OnEnChangeEditxml)
	ON_EN_CHANGE(IDC_EDITOUTPUT, &CHyperLogoDlg::OnEnChangeEditoutput)
	ON_BN_CLICKED(IDC_CHECKOVERWRITE, &CHyperLogoDlg::OnBnClickedCheckoverwrite)
END_MESSAGE_MAP()

std::wstring CHyperLogoDlg::s2ws(const std::string& s)
{
 int len;
 int slength = (int)s.length() + 1;
 len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
 wchar_t* buf = new wchar_t[len];
 MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
 std::wstring r(buf);
 delete[] buf;
 return r;
}

// CHyperLogoDlg message handlers

BOOL CHyperLogoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_Preview.AssertValid();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	hPreview = 0;
	
	hlSettings.autoUpdatePreview = false;
	rgbColourToMagick(hlSettings.strokeColourRGB, hlSettings.strokeColour, 
		0, 0, 0, 1 );
	rgbColourToMagick(hlSettings.fillColourRGB, hlSettings.fillColour, 
		0, 0, 0, 1 );
	

	hlSettings.fontSettings.name = "Arial";
	hlSettings.fontSettings.file = "ARIAL.TTF";
	hlSettings.fontSettings.size = 60;

	editFontSize.SetWindowTextW(_T("60"));
	hlSettings.fontSettings.strokeWidth = 2;
	editStrokeWidth.SetWindowTextW(_T("2"));

	hlSettings.overwriteFiles = false;
	editPreviewTxt.SetWindowTextW(_T("HyperLogo Preview"));

	CheckAutoUpdate.SetCheck(TRUE);
	OnBnClickedCheckAutoUpdate();

	//find windows dir
	char* winDir;
	size_t requiredSize;
	getenv_s( &requiredSize, NULL, 0, "WINDIR");
	winDir = (char*) malloc(requiredSize * sizeof(char));
	getenv_s( &requiredSize, winDir, requiredSize, "WINDIR" );

	std::string findDir = winDir;
	findDir+= "\\fonts\\*.*";

	std::wstring wfindDir = s2ws(findDir);

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(wfindDir.c_str(), &findFileData);

	if(hFind  == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox (_T("No windows fonts found"));
	} 
	else 
	{
		while(FindNextFile(hFind, &findFileData))
		{
			std::wstring wFname= findFileData.cFileName;
			std::transform(wFname.begin(), wFname.end(), wFname.begin(), tolower);
			if (wcsstr(wFname.c_str(),_T(".ttf")) || wcsstr(wFname.c_str(),_T(".otf")) )
				fontCombo.AddString(findFileData.cFileName);
		}
	}

	isOutput = false;
	outFileNo=0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHyperLogoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	if (isOutput)
		updateOutput();
	
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHyperLogoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHyperLogoDlg::OnBnClickedGenerateLogos()
{
	if ( isOutput == true )
	{
		stopGenerating(true);
		return;
	}
	 std::vector<gameInfo*>::iterator gameIt = gameVec.begin();

	 for ( ; gameIt < gameVec.end(); gameIt++)
	 {
		delete (*gameIt);
	 }
	 gameVec.clear();

	TiXmlDocument doc( hlSettings.xmlFile.c_str() );
	if (!doc.LoadFile())
	{
		AfxMessageBox(_T("Error loading/parsing HyperSpin XML"));
		return;
	}

	TiXmlElement *root = NULL;
	if (! (root = doc.FirstChildElement("menu")) )
	{
		AfxMessageBox(_T("Error loading/parsing HyperSpin XML"));
		return;
	}

	TiXmlElement *child = NULL;
	child = root->FirstChildElement("game");

	while(child)
	{
		gameInfo *gInfo = new gameInfo;
		gInfo->name = child->Attribute("name");

		TiXmlElement *child2 = NULL;
		child2 = child->FirstChildElement("description");
			gInfo->description = child2->GetText();

		gameVec.push_back(gInfo);
		child = child->NextSiblingElement("game");
	}

	updateOutput();
}


void CHyperLogoDlg::updateOutput()
{
	if (isOutput == false)
	{
		isOutput = true;
		outFileNo=0;
	}
	
	gameIter = gameVec.begin();
	gameIter += outFileNo;
	
	if (gameIter == gameVec.end())
	{
		stopGenerating(false);
		return;
	}


	bool writeFile = hlSettings.overwriteFiles;
	
	std::string outFile = hlSettings.outputDir;
	outFile += "\\";
	outFile += (*gameIter)->name;
	outFile += ".png";

	if (writeFile == false)
	{
		//test if file exists
		std::ifstream inp;
		inp.open(outFile.c_str(), std::ifstream::in);
		inp.close();
		if (inp.fail())
			writeFile = true;
	}

	if(writeFile)
	{

		static bool initd = false;

		static Magick::Image my_image( Magick::Geometry(1024,768), Magick::Color("white"));

		//if (!initd)
		//{
			
			//initd=true;
		//}

		std::string strStd = (*gameIter)->description;

		if (strStd.size() > 15)
		{
			strStd.replace(15, strStd.size(), "..");
		}

		processImage(my_image, strStd, true);
		Magick::TypeMetric tm;
		my_image.fontTypeMetrics(strStd, &tm);
		my_image.crop(Magick::Geometry(tm.textWidth(), tm.textHeight()));
		
		my_image.magick("png");
		
		my_image.write(outFile.c_str());
	}

	outFileNo++;
	float pos = ((float)outFileNo / gameVec.size() )*100.f;
	progressOutput.SetPos((int)pos);
	//AfxGetMainWnd()->SendMessage(WM_PAINT);
	generateButton.SetWindowTextW(_T("Stop"));
	AfxGetMainWnd()->Invalidate(false);
	AfxGetMainWnd()->SetRedraw(true);
	//AfxGetMainWnd()->UpdateWindow();

}
void CHyperLogoDlg::OnBnClickedBtnXML()
{
	CFileDialog fileDialog(true, NULL, _T("Mame.xml"), 
		OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||"),
		AfxGetMainWnd());

	if (fileDialog.DoModal() == IDOK)
	{
		EditXMLFile.SetWindowTextW(fileDialog.GetPathName());
	}

}

void CHyperLogoDlg::OnBnClickedBtnoutput()
{
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));

	bi.ulFlags   = BIF_USENEWUI;
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.lpszTitle = _T("Output Folder");

	LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

	if(pIDL != NULL)
	{
		LPWSTR buffer= (LPWSTR)malloc(_MAX_PATH);
		
		if(::SHGetPathFromIDList(pIDL, buffer) != 0)
		{
			editOutputDir.SetWindowTextW(buffer);
		}

		// free the item id list

		free(buffer);
		CoTaskMemFree(pIDL);
	}

}

// Converts ImageMagick++ Image to HBITMAP
void CHyperLogoDlg::CreateBitmapFromMagick(HDC hDC, Magick::Image &magick_image)
{
	void*					out_pixel_pointer		= 0;
	unsigned char*			pixel_bits_pointer		= 0;
	Magick::PixelPacket*	pixel_packet_pointer	= 0;
	unsigned int			width, height, c1, c2;
	BITMAPINFO				bitmap_info				= {0};

	if (hPreview)
	{
		DeleteObject(hPreview);
        hPreview = 0;
	}

	if(!magick_image.isValid())
	{	return;
	}

	// Get geometry	
	width			= magick_image.size().width();
	height			= magick_image.size().height();

	// Get pixel packet
	pixel_packet_pointer = magick_image.getPixels(0,0,width,height);

	// Allocate pixel bits 
	pixel_bits_pointer = new unsigned char[width*height*3];

	// Convert rgb pixel data
	c1 = c2 = 0;	
	for(unsigned int i=0; i<width; i++)
	{	for(unsigned int j=0; j<height; j++)
		{	
			pixel_bits_pointer[c1+0] = (unsigned char)(((float)pixel_packet_pointer[c2].blue/65535) * 255);
			pixel_bits_pointer[c1+1] = (unsigned char)(((float)pixel_packet_pointer[c2].green/65535) * 255);
			pixel_bits_pointer[c1+2] = (unsigned char)(((float)pixel_packet_pointer[c2].red/65535) * 255);			
			c1+= 3;
			c2++;
		}
	}

	//Set bmp header info
	bitmap_info.bmiHeader.biBitCount	= 24;
    bitmap_info.bmiHeader.biHeight		= height;
    bitmap_info.bmiHeader.biWidth		= width;
    bitmap_info.bmiHeader.biPlanes		= 1;
    bitmap_info.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);   
    hPreview						= CreateDIBSection(hDC, (BITMAPINFO *)&
												bitmap_info, DIB_RGB_COLORS, 
												(void **)&out_pixel_pointer, 0, 0);
	if(!hPreview)
	{	delete [] pixel_bits_pointer;
		return; 
	}

	// Set the bits
    SetBitmapBits(hPreview, width*height*3, pixel_bits_pointer);

	// Cleanup and return
	delete [] pixel_bits_pointer;
	return;
}
void CHyperLogoDlg::processImage(Magick::Image &image, std::string &string, bool isTransparent)
{
	image.size(Magick::Geometry(1000,230));
	image.erase();
	image.fillColor(Magick::Color("blue"));
	if (isTransparent)
	{
		image.transparent(Magick::Color("white"));
	}
	std::string filename = "c:/windows/fonts/" + hlSettings.fontSettings.file;

	image.font(filename);
	image.fontPointsize( hlSettings.fontSettings.size );
	image.strokeColor(hlSettings.strokeColour);
	image.strokeWidth(hlSettings.fontSettings.strokeWidth);
	image.fillColor(hlSettings.fillColour);

	if (string.size() > 15)
	{
		string.replace(15, string.size(), "..");
	}
	image.annotate(string, Magick::NorthWestGravity);
}

void CHyperLogoDlg::OnBnClickedBtpreview()
{
	static Magick::Image my_image( Magick::Geometry(500,100), Magick::Color(getQuantum(227), getQuantum(229), getQuantum(233)));

	CString cStr;
	editPreviewTxt.GetWindowTextW(cStr);

	 // Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString (cStr);
	//construct a std::string using the LPCSTR input
	std::string strStd = (pszConvertedAnsiString);

	processImage(my_image, strStd, false);
	CreateBitmapFromMagick( AfxGetMainWnd()->GetDC()->GetSafeHdc(), my_image);
	m_Preview.SetBitmap( hPreview );
}


void CHyperLogoDlg::OnEnChangeEditpreviewtxt()
{
	if (hlSettings.autoUpdatePreview)
		OnBnClickedBtpreview();
}

void CHyperLogoDlg::OnBnClickedCheckAutoUpdate()
{
	hlSettings.autoUpdatePreview = !!CheckAutoUpdate.GetCheck();
	if (hlSettings.autoUpdatePreview)
		OnBnClickedBtpreview();
}

void CHyperLogoDlg::OnBnClickedBtnFill()
{
	static COLORREF rgbCustom[16];
	
	CColorDialog dlg(RGB(hlSettings.fillColourRGB.r, hlSettings.fillColourRGB.g, hlSettings.fillColourRGB.b), CC_FULLOPEN, AfxGetMainWnd());

	memcpy(dlg.m_cc.lpCustColors, rgbCustom, sizeof(rgbCustom));

	

	if(dlg.DoModal() == IDOK)
	{
		rgbColourToMagick(hlSettings.fillColourRGB, hlSettings.fillColour, 
			GetRValue(dlg.GetColor()), GetGValue(dlg.GetColor()), GetBValue(dlg.GetColor()), 1 );
		if (hlSettings.autoUpdatePreview)
			OnBnClickedBtpreview();
	}
}

void CHyperLogoDlg::OnBnClickedBtnStroke()
{
	CColorDialog dlg(RGB(hlSettings.strokeColourRGB.r, hlSettings.strokeColourRGB.g, hlSettings.strokeColourRGB.b), CC_FULLOPEN, AfxGetMainWnd());

	if(dlg.DoModal() == IDOK)
	{
		rgbColourToMagick(hlSettings.strokeColourRGB, hlSettings.strokeColour, 
			GetRValue(dlg.GetColor()), GetGValue(dlg.GetColor()), GetBValue(dlg.GetColor()), 1 );
		if (hlSettings.autoUpdatePreview)
			OnBnClickedBtpreview();
	}
}

void CHyperLogoDlg::rgbColourToMagick(rgbColour &rgbCol, Magick::Color &magCol, int r, int g, int b, int a)
{
	rgbCol.r = r;
	rgbCol.g = g;
	rgbCol.b = b;
	rgbCol.a = a;
	magCol.redQuantum ( getQuantum(r));
	magCol.greenQuantum ( getQuantum(g));
	magCol.blueQuantum ( getQuantum(b));
	magCol.alphaQuantum ( getQuantum(a));

}
//converts an 0-255 colour value to a quantum
MagickCore::Quantum CHyperLogoDlg::getQuantum(int val)
{
	double result = (double)val / 255;
	return Magick::Color::scaleDoubleToQuantum(result);

}

void CHyperLogoDlg::OnCbnSelChangeFonts()
{
	
	if( fontCombo.GetCurSel() < 0 ) return;
	CString buff;
	fontCombo.GetLBText(fontCombo.GetCurSel(), buff);
	CT2CA pszConvertedAnsiString (buff);

	if(pszConvertedAnsiString.m_szBuffer!="")
	{
		hlSettings.fontSettings.file = (pszConvertedAnsiString);
		if (hlSettings.autoUpdatePreview)
			OnBnClickedBtpreview();
	}
}

void CHyperLogoDlg::OnBnClickedQuit()
{
	 std::vector<gameInfo*>::iterator gameIt = gameVec.begin();

	 for ( ; gameIt < gameVec.end(); gameIt++)
	 {
		delete (*gameIt);
	 }
	 gameVec.clear();
	isOutput=false;
	MagickCore::MagickCoreTerminus();
	// AfxGetMainWnd()->SendMessage(WM_CLOSE);

	
}

void CHyperLogoDlg::OnEnChangeEditfontsize()
{
	CString cStr;
			
	int len = editFontSize.LineLength();
	if (len > 0)
	{
		editFontSize.GetWindowTextW(cStr);
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString (cStr);
		//construct a std::string using the LPCSTR input
		std::string strStd = (pszConvertedAnsiString);
		hlSettings.fontSettings.size = atoi(strStd.c_str());
		if (hlSettings.autoUpdatePreview)
			OnBnClickedBtpreview();
	}
	
}

void CHyperLogoDlg::OnEnChangeEditstrokewidth()
{
	CString cStr;
			
	int len = editStrokeWidth.LineLength();
	if (len > 0)
	{
		editStrokeWidth.GetWindowTextW(cStr);
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString (cStr);
		//construct a std::string using the LPCSTR input
		std::string strStd = (pszConvertedAnsiString);
		hlSettings.fontSettings.strokeWidth = atoi(strStd.c_str());
		if (hlSettings.autoUpdatePreview)
			OnBnClickedBtpreview();
	}
}

void CHyperLogoDlg::OnEnChangeEditxml()
{
	CString cStr;
			
	int len = EditXMLFile.LineLength();
	if (len > 0)
	{
		EditXMLFile.GetWindowTextW(cStr);
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString (cStr);
		//construct a std::string using the LPCSTR input
		std::string strStd = (pszConvertedAnsiString);
		hlSettings.xmlFile = strStd.c_str();
	}
}

void CHyperLogoDlg::OnEnChangeEditoutput()
{
	CString cStr;
			
	int len = editOutputDir.LineLength();
	if (len > 0)
	{
		editOutputDir.GetWindowTextW(cStr);
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString (cStr);
		//construct a std::string using the LPCSTR input
		std::string strStd = (pszConvertedAnsiString);
		hlSettings.outputDir = strStd;
	}
}

void CHyperLogoDlg::OnBnClickedCheckoverwrite()
{
	hlSettings.overwriteFiles = !!CheckOverwrite.GetCheck();
}

void CHyperLogoDlg::stopGenerating(bool userCancelled)
{
	if (isOutput == true)
	{
		isOutput = false;
		outFileNo=0;
		if ( userCancelled )
			AfxMessageBox(_T("Stopped generation upon users request"));
		else
			AfxMessageBox(_T("Finished!"));
		generateButton.SetWindowTextW(_T("Generate Logos"));
		progressOutput.SetPos(0);
	}
}
