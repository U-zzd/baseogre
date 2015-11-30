//////////////////////////////////////////////
// View.h

#ifndef VIEW_H
#define VIEW_H
#include "Ogre.h"
#include "ogreeuler.h"
#include "kinecthelper.h"
#include "debugdrawer.h"

// NOTE: for MS compilers you will need the DirectX SDK v9
//       for Dev-C++ you will need the DirecX v9.0c DevPak



//#define UWM_RESIZE     (WM_APP + 0x0001)  // Message - a copy of WM_SIZE info forwarded on to the DirectX window.




///////////////////////////////////////////////////////
// Definition of the CDXView class
//  CDXView is the view window of CMainFrame. It is the
//   parent window of CDX.
//
//  Note: this class has two nested classes:
//  CDX			- the child window that renders the DirectX scene
//  CDXThread	- A seperate thread with the DirectX message loop

class CDXView : public CWnd
{
	// CDX nested class within CDXView
	class CDX : public CWnd , public Ogre::FrameListener
	{
	public:
		CDX();
		virtual ~CDX();

		HRESULT InitOgre(HWND hWnd);
		HRESULT InitGeometry();
		void Render();
		void SetupDefaultRenderStates();
		void SetupMatrices();
		HRESULT InitResources();

	protected:
		virtual	int  OnCreate(LPCREATESTRUCT pcs);
		virtual void OnDestroy();
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void PreCreate(CREATESTRUCT &cs);
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		// Ogre::FrameListener

		virtual bool frameStarted(const Ogre::FrameEvent& evt);
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		virtual bool frameEnded(const Ogre::FrameEvent& evt);

		void setupAnimations();
		void setupBone(const Ogre::String& name, const Ogre::Quaternion& q);
		void setupBone2(const Ogre::String& name, const Ogre::Quaternion& q);

		
		//q is the absolute  orientation  
		//internally this method should first retrieve the parent bone and substract the parent orientation
		void  applyBoneTransform(int boneIndex, Ogre::Quaternion& q);
		Ogre::Quaternion calculateBoneTransform(int boneIndex,  Ogre::Quaternion& q);



	private:
	
		bool m_First;
		int mDrawOrient;
		int m_boneIndex;
		bool m_mouseDown;
		bool m_centerDown;
		bool m_showModel;
		Ogre::Euler m_boneOrients[18];
		std::vector<Ogre::String*> m_BoneNames;
		std::vector<Ogre::Euler> m_BoneEulers;
		std::vector<Ogre::Quaternion> m_BoneQuats;
		
		std::vector<bool> m_BoneTracked;


		KinectHelper m_kinectHelper;

		Ogre::Camera*m_Camera;
		Ogre::RenderWindow*m_RenderWindow;
		Ogre::Root *m_ogreRoot;
		//Ogre::AnimationState* mAnimationState;
		Ogre::Entity *m_entity;
	};
	
	// CDXThread nested class within CDXView
	class CDXThread : public CWinThread
	{
	public:
		CDXThread() {}
		virtual ~CDXThread()  {	}

		virtual BOOL InitInstance();
		virtual int MessageLoop();

	private:
	
		CDX* m_pDX;		// OGre window


	};

public:
	CDXView() {}
	virtual ~CDXView();

	CDX& GetDX() const { return const_cast<CDX&>(m_DX); }

protected:
	virtual int OnCreate(LPCREATESTRUCT pcs);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CDX m_DX;
	CDXThread m_DXThread;
};


#endif
