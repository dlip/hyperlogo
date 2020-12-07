// HyperLogoDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

struct rgbColour
{
	int r, g, b, a;
};
struct FontSettings
{
	std::string name;
	std::string file;
	int size, strokeWidth;
};

struct hyperLogoSettings
{
	bool autoUpdatePreview;
	Magick::Color fillColour;
	rgbColour fillColourRGB;
	Magick::Color strokeColour;
	rgbColour strokeColourRGB;
	FontSettings fontSettings;
	std::string xmlFile, outputDir;
	bool overwriteFiles;
};

struct gameInfo
{
	std::string name, description;
};

// CHyperLogoDlg dialog
class CHyperLogoDlg : public CDialog
{
// Construction
public:
	CHyperLogoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HYPERLOGO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void CreateBitmapFromMagick(HDC hDC, Magick::Image &magick_image);
	MagickCore::Quantum CHyperLogoDlg::getQuantum(int val);
	void rgbColourToMagick(rgbColour &rgbCol, Magick::Color &magCol, int r, int g, int b, int a);

	afx_msg void OnBnClickedGenerateLogos();
	afx_msg void OnBnClickedBtnXML();
	afx_msg void OnBnClickedBtnoutput();
	afx_msg void OnBnClickedBtpreview();
	afx_msg void OnBnClickedCheckoverwrite();
	afx_msg void OnBnClickedBtnFill();
	afx_msg void OnBnClickedBtnStroke();
	afx_msg void OnBnClickedCheckAutoUpdate();
	afx_msg void OnEnChangeEditpreviewtxt();

private:
	std::wstring s2ws(const std::string& s);
	HBITMAP hPreview;
	CStatic m_Preview;
	Magick::Image *imgPreview;
	CEdit editPreviewTxt;
	CButton CheckAutoUpdate;
	hyperLogoSettings hlSettings;
public:
	void processImage(Magick::Image &image, std::string &string, bool isTransparent=true);
	void updateOutput();
	afx_msg void OnCbnSelChangeFonts();
	CComboBox fontCombo;
	afx_msg void OnBnClickedQuit();
	afx_msg void OnEnChangeEditfontsize();
	CEdit editFontSize;
	afx_msg void OnEnChangeEditstrokewidth();
	CEdit editStrokeWidth;
	std::vector<gameInfo*> gameVec;
	std::vector<gameInfo*>::iterator gameIter;

	afx_msg void OnEnChangeEditxml();
	CEdit EditXMLFile;
	afx_msg void OnEnChangeEditoutput();
	CEdit editOutputDir;
	CProgressCtrl progressOutput;
	int outFileNo;
	bool isOutput;
	CButton CheckOverwrite;
	afx_msg void stopGenerating(bool userCancelled);
	CButton generateButton;
};
