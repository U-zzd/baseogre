//////////////////////////////////////////////
// View.cpp

// Includes backbuffer resizing suggested by Guillaume Werlé

// NOTE: for MS compilers you will need the DirectX SDK v9
//       for Dev-C++ you will need the DirecX v9.0c DevPak


#include "stdafx.h"
#include "DXApp.h"
#include <OgreConfigFile.h>
#include <OgreGLRenderSystem.h>

/////////////////////////////////////////////////
// Definitions for the CDXView class
//
BOOL CDXView::CDXThread::InitInstance()
{
	// This function runs when the thread starts

	CMainFrame& Frame = GetDXApp().GetMainFrame();
	CDXView& DXView = Frame.GetDXView();
	CDX& DX = DXView.GetDX();
	
	// assign the m_pDX member variable
	m_pDX = &DX;

	// Create the DX window
	DX.Create(DXView);

	return TRUE;	// return TRUE to run the message loop
}

int CDXView::CDXThread::MessageLoop()
// Here we override CWinThread::MessageLoop to accommodate the special needs of DirectX
{
	MSG Msg;
	ZeroMemory(&Msg, sizeof(MSG));
	while( Msg.message != WM_QUIT )
	{
		if ( PeekMessage(&Msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&Msg);
			::DispatchMessage(&Msg);
		}
		else
		{
			// Force thread to yield
			if (WAIT_TIMEOUT == ::WaitForSingleObject(*this, 1))
				m_pDX->Render();
		}
	}

	return LOWORD(Msg.wParam);
} 

/////////////////////////////////////////////////
// Definitions for the CDXView class
//
CDXView::CDX::CDX() : m_Camera(NULL), m_RenderWindow(NULL), m_First(true), m_ogreRoot(NULL)
{
	m_mouseDown = false;
	m_boneIndex = 0;
	m_entity = NULL;

}

CDXView::CDX::~CDX()
{
	
	m_Camera = NULL;
	m_RenderWindow = NULL;


}

HRESULT CDXView::CDX::InitOgre( HWND hWnd )
// Initializes OGRE
{

	
	CRect rect = GetClientRect();
	HWND aHwnd = GetHwnd();


	//intialize ogre
	Ogre::Root *root = new Ogre::Root("", "", "ogre.log");
	Ogre::RenderSystem  * renderSystem = new Ogre::GLRenderSystem();

	root->addRenderSystem(renderSystem);
	root->setRenderSystem(renderSystem);
	m_ogreRoot = root;
	// initialise root
	root->initialise(false);
	Ogre::NameValuePairList parms;
	char buffer[33];
	itoa((unsigned int)aHwnd, buffer, 10);
	Ogre::String s(buffer);

	parms["externalWindowHandle"] =s;
	m_RenderWindow = root->createRenderWindow("Ogre in MFC", rect.Width(), rect.Height(), false, &parms);



	



	Ogre::SceneManager *SceneManager = NULL;
	SceneManager = root->createSceneManager(Ogre::ST_GENERIC, "MFCOgre");
	SceneManager->setAmbientLight(Ogre::ColourValue(0.9, 0.9, 0.9));


	// Create the camera
	m_Camera = SceneManager->createCamera("Camera");
	m_Camera->setNearClipDistance(0.5);
	m_Camera->setFarClipDistance(5000);
	m_Camera->setCastShadows(false);
	m_Camera->setUseRenderingDistance(true);
//	m_Camera->setPosition(Ogre::Vector3(200.0, 50.0, 100.0));
	m_Camera->setPosition(Ogre::Vector3(2.0, 2.0, 5.0));

	Ogre::SceneNode *CameraNode = NULL;
	CameraNode = SceneManager->getRootSceneNode() ->createChildSceneNode("CameraNode");


	Ogre::Viewport* Viewport = NULL;

	if (0 == m_RenderWindow->getNumViewports()){
		Viewport = m_RenderWindow->addViewport(m_Camera);
		Viewport->setBackgroundColour(Ogre::ColourValue(0.8f, 1.0f,
			0.8f));
	}

	m_Camera->setAspectRatio(Ogre::Real(rect.Width()) /
		Ogre::Real(rect.Height()));



    return S_OK;
}

HRESULT CDXView::CDX::InitGeometry()
// Creates the scene geometry.
{
    //load the ogre robot
	

	Ogre::SceneManager *SceneManager = m_Camera->getSceneManager();



//	m_entity = SceneManager->createEntity("Robot", "robot.mesh");
	m_entity = SceneManager->createEntity("Robot", "Geometry1.mesh");


	Ogre::SceneNode *RobotNode = SceneManager->getRootSceneNode() ->createChildSceneNode();
	RobotNode->attachObject(m_entity);

	//m_entity->setMaterialName("Examples/Fish"); //"jaiquaDualQuatTest"


	Ogre::AxisAlignedBox Box = m_entity->getBoundingBox();
	Ogre::Vector3 Center = Box.getCenter();
	m_Camera->lookAt(Center);

	m_ogreRoot->addFrameListener(this);
	//start the wal animation
	/*
	mAnimationState = RobotEntity->getAnimationState("Walk");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);
	*/
	setupAnimations();

    return S_OK;
}

void CDXView::CDX::setupBone2(const Ogre::String& name, const Ogre::Quaternion& q)
{
	Ogre::Bone* bone = m_entity->getSkeleton()->getBone(name);
	bone->setManuallyControlled(true);

//	bone->getOrientation();
	bone->setInheritOrientation(true);

	//bone->resetOrientation();
	//bone->setOrientation(q);

	bone->setInitialState();
}

void CDXView::CDX::setupBone(const Ogre::String& name, const Ogre::Quaternion& q)
{
	
	/*Ogre::Bone* bone = m_entity->getSkeleton()->getBone(name);
	bone->setManuallyControlled(true);
	bone->setInheritOrientation(false);

	bone->resetOrientation();
	bone->setOrientation(q);

	bone->setInitialState();
	*/
	Ogre::Bone* bone = m_entity->getSkeleton()->getBone(name);
	bone->setManuallyControlled(true);
	bone->setInheritOrientation(false);

	bone->resetOrientation();
	bone->setOrientation(q);

	bone->setInitialState();


}



void CDXView::CDX::setupAnimations()
{
	// this is very important due to the nature of the exported animations
	//entidadPersonaje->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);	

	/*
	Ogre::String bones[] =
	{ "Joint1", 
	  "Joint2", 
	  "Joint3", 
	  "Joint4", 
	  "Joint5", 
	  "Joint6", 
	  "Joint7",
	  "Joint8", 
	  "Joint9", 
	  "Joint10", 
	  "Joint11", 
	  "Joint12", 
	  "Joint13",
	  "Joint14",
	  "Joint15",
	  "Joint16",
	  "Joint17",
	  "Joint18"
	};
	int boneCount = 18;
	const int Joint1 = 0;
	const int Joint2 = 1;
	const int Joint3 = 2;
	const int Joint4 = 3;
	const int Joint5 = 4;
	const int Joint6 = 5;
	const int Joint7 = 6;
	const int Joint8 = 7;
	const int Joint9 = 8;
	const int Joint10 = 9;
	const int Joint11 = 10;
	const int Joint12 = 11;
	const int Joint13 = 12;
	const int Joint14 = 13;
	const int Joint15 = 14;
	const int Joint16 = 15;
	const int Joint17 = 16;
	const int Joint18 = 17;

	m_kinectHelper.setBoneMapping(JointType_SpineBase, Joint1);
	m_kinectHelper.setBoneMapping(JointType_SpineMid, Joint10);
//	m_kinectHelper.setBoneMapping(JointType_SpineShoulder, VisualSceneNode37);
	m_kinectHelper.setBoneMapping(JointType_Neck, Joint11);
	m_kinectHelper.setBoneMapping(JointType_Head, Joint12);

	//m_kinectHelper.setBoneMapping(JointType_ShoulderRight, VisualSceneNode41);
	m_kinectHelper.setBoneMapping(JointType_ElbowRight, Joint16);
	m_kinectHelper.setBoneMapping(JointType_WristRight, Joint17);

	m_kinectHelper.setBoneMapping(JointType_KneeRight, Joint6);
	m_kinectHelper.setBoneMapping(JointType_AnkleRight, Joint7);


	//m_kinectHelper.setBoneMapping(JointType_ShoulderLeft, VisualSceneNode15);
	m_kinectHelper.setBoneMapping(JointType_ElbowLeft, Joint13);
	m_kinectHelper.setBoneMapping(JointType_WristLeft, Joint14);

	m_kinectHelper.setBoneMapping(JointType_KneeLeft, Joint2);
	m_kinectHelper.setBoneMapping(JointType_AnkleLeft, Joint3);

	*/
	
	Ogre::String bones[] =
	{
		"VisualSceneNode1",
		"VisualSceneNode2",
		"VisualSceneNode3",
		"VisualSceneNode4",
		"VisualSceneNode5",
		"VisualSceneNode6",
		"bonefootl",
		"VisualSceneNode8",
		"VisualSceneNode9",
		"VisualSceneNode10",
		"bonefootr",
		"VisualSceneNode12",
		"VisualSceneNode14",
		"VisualSceneNode15",
		"VisualSceneNode36",
		"VisualSceneNode17",
		"bonendingl",
		"VisualSceneNode37",
		"VisualSceneNode38",
		"VisualSceneNode39",
		"VisualSceneNode40",
		"VisualSceneNode41",
		"VisualSceneNode62",
		"VisualSceneNode43",
		"boneendingr" };
	
	const int VisualSceneNode1 = 0;
	const int VisualSceneNode2 = 1;
	const int VisualSceneNode3 = 2;
	const int VisualSceneNode4 = 3;
	const int VisualSceneNode5 = 4;
	const int VisualSceneNode6 = 5;
	const int bonefootl = 6;
	const int VisualSceneNode8 = 7;
	const int VisualSceneNode9 = 8;
	const int VisualSceneNode10 = 9;
	const int bonefootr = 10;
	const int VisualSceneNode12 = 11;
	const int VisualSceneNode14 = 12;
	const int VisualSceneNode15 = 13;
	const int VisualSceneNode36 = 14;
	const int VisualSceneNode17 = 15;
	const int bonendingl = 16;
	const int VisualSceneNode37 = 17;
	const int VisualSceneNode38 = 18;
	const int VisualSceneNode39 = 19;
	const int VisualSceneNode40 = 20;
	const int VisualSceneNode41 = 21;
	const int VisualSceneNode62 = 22;
	const int VisualSceneNode43 = 23;
	const int boneendingr = 24;
	int boneCount = 25;


	m_kinectHelper.setBoneMapping(JointType_SpineBase, VisualSceneNode1);	

	/*
	m_kinectHelper.setBoneMapping(JointType_SpineMid, VisualSceneNode12);
	m_kinectHelper.setBoneMapping(JointType_SpineShoulder, VisualSceneNode37);
	m_kinectHelper.setBoneMapping(JointType_Neck, VisualSceneNode38);
	m_kinectHelper.setBoneMapping(JointType_Head, VisualSceneNode39);

	m_kinectHelper.setBoneMapping(JointType_ShoulderRight, VisualSceneNode41);
	m_kinectHelper.setBoneMapping(JointType_ElbowRight, VisualSceneNode62);
	m_kinectHelper.setBoneMapping(JointType_WristRight, VisualSceneNode43);

	
	m_kinectHelper.setBoneMapping(JointType_ShoulderLeft, VisualSceneNode15);
	m_kinectHelper.setBoneMapping(JointType_ElbowLeft, VisualSceneNode36);
	m_kinectHelper.setBoneMapping(JointType_WristLeft, VisualSceneNode17);
	m_kinectHelper.setBoneMapping(JointType_KneeRight, VisualSceneNode8);
	m_kinectHelper.setBoneMapping(JointType_AnkleRight, VisualSceneNode9);
	m_kinectHelper.setBoneMapping(JointType_KneeLeft, VisualSceneNode4);
	m_kinectHelper.setBoneMapping(JointType_AnkleLeft, VisualSceneNode5);

	
	*/



	for (int i = 0; i < boneCount; i++)
	{
		m_BoneNames.push_back(new Ogre::String(bones[i]));
		m_BoneEulers.push_back (Ogre::Euler());
	
	}

	Ogre::AnimationStateSet* animations = m_entity->getAllAnimationStates();
	Ogre::AnimationStateIterator it = animations->getAnimationStateIterator();

	//set all to manualy controlled
	Ogre::Quaternion ki = Ogre::Quaternion::IDENTITY;
	Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;
	
	for (int i = 0; i < m_BoneNames.size(); i++)
	{
		Ogre::String& boneName = *m_BoneNames[i];
		setupBone2( boneName, q);
		//setupBone(boneName, q);

	}
	
	/*

	Ogre::Quaternion q2, q3;
	Ogre::Vector3 xAxis, yAxis, zAxis;
	q.FromAngleAxis(Ogre::Degree(90), Ogre::Vector3(0, 0, 1));
	q.ToAxes(xAxis, yAxis, zAxis);
	q2.FromAngleAxis(Ogre::Degree(90), xAxis);
	setupBone("VisualSceneNode36", ki); //shoulder left


	q.FromAngleAxis(Ogre::Degree(90), Ogre::Vector3(0, 0, 1));
	q.ToAxes(xAxis, yAxis, zAxis);
	q2.FromAngleAxis(Ogre::Degree(90), xAxis);
	setupBone("VisualSceneNode62", ki); //shoulder right

	q.FromAngleAxis(Ogre::Degree(90), Ogre::Vector3(0, 0, -1));
	q2.FromAngleAxis(Ogre::Degree(45), Ogre::Vector3(0, -1, 0));

	setupBone("VisualSceneNode17", ki); //wrist left

	q.FromAngleAxis(Ogre::Degree(90), Ogre::Vector3(0, 0, 1));
	setupBone("VisualSceneNode43", ki); //wrist right

	q.FromAngleAxis(Ogre::Degree(180), Ogre::Vector3(0, 1, 0));
	//setupBone("Chest", q);
	setupBone("VisualSceneNode12", q);  //stomach
	q.FromAngleAxis(Ogre::Degree(180), Ogre::Vector3(1, 0, 0));
	q2.FromAngleAxis(Ogre::Degree(180), Ogre::Vector3(0, 1, 0));
	setupBone("VisualSceneNode4", q*q2); //femur left
	setupBone("VisualSceneNode8", q*q2); //femur  right
	setupBone("VisualSceneNode5", q*q2); //knee left
	setupBone("VisualSceneNode9", q*q2); //knee right

	q = Ogre::Quaternion::IDENTITY;

	setupBone("VisualSceneNode1", q);



	*/


	while (it.hasMoreElements())
	{
		Ogre::AnimationState* animState = it.getNext();
		animState->setLoop(true);

		Ogre::Animation* anim = m_entity->getSkeleton()->getAnimation(animState->getAnimationName());

		for (int i = 0; i < m_BoneNames.size(); i++)
		{
			Ogre::String& boneName = *m_BoneNames[i];

			anim->destroyNodeTrack(m_entity->getSkeleton()->getBone(boneName)->getHandle());


		}
	}




}



void  CDXView::CDX::transformBone(const Ogre::String& modelBoneName, Ogre::Euler& euler)
{
	// Get the model skeleton bone info
	Ogre::Skeleton* skel = m_entity->getSkeleton();
	Ogre::Bone* bone = skel->getBone(modelBoneName);
	Ogre::Quaternion qI = Ogre::Quaternion::IDENTITY;; // bone->getInitialOrientation();
	Ogre::Quaternion newQ = Ogre::Quaternion::IDENTITY;

	{
   	    newQ = euler.toQuaternion();
	 
		bone->resetOrientation(); //in order for the conversion from world to local to work.
	//	newQ = bone->convertWorldToLocalOrientation(newQ);
		bone->setOrientation(newQ*qI);
	}
}
/*
void  CDXView::CDX::transformBone(const Ogre::String& modelBoneName, Ogre::Euler& euler)
{
	Ogre::Skeleton* skel = m_entity->getSkeleton();
	Ogre::Bone* bone = skel->getBone(modelBoneName);
	Ogre::Quaternion qI =  bone->getInitialOrientation();
	Ogre::Quaternion newQ = Ogre::Quaternion::IDENTITY;

	{
		newQ = euler.toQuaternion();

		bone->resetOrientation(); //in order for the conversion from world to local to work.
		//newQ = bone->convertWorldToLocalOrientation(newQ);
		bone->setOrientation(newQ*qI);
	}
	*/
	/*
	xn::SkeletonCapability pUserSkel = objeto_kinect->g_UserGenerator.GetSkeletonCap();
	XnSkeletonJointOrientation jointOri;
	pUserSkel.GetSkeletonJointOrientation(objeto_kinect->UserId, skelJoint, jointOri);

	static float deg = 0;
	if (jointOri.fConfidence == 1)
	{
		Ogre::Matrix3 matOri(jointOri.orientation.elements[0], -jointOri.orientation.elements[1], jointOri.orientation.elements[2],
			-jointOri.orientation.elements[3], jointOri.orientation.elements[4], -jointOri.orientation.elements[5],
			jointOri.orientation.elements[6], -jointOri.orientation.elements[7], jointOri.orientation.elements[8]);
		Ogre::Quaternion q;
		newQ.FromRotationMatrix(matOri);
		bone->resetOrientation(); //in order for the conversion from world to local to work.
		newQ = bone->convertWorldToLocalOrientation(newQ);
		bone->setOrientation(newQ*qI);
	}
	*/
/*
}
*/

bool CDXView::CDX::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	
	/*if (mAnimationState)
		mAnimationState->addTime(evt.timeSinceLastFrame);
		*/

	if (m_entity && !m_First)
	{

		if (m_kinectHelper.initialized())
		{

			for (int i = 0; i < m_kinectHelper.m_boneMapping.size(); i++)
			{

				int index = m_kinectHelper.m_boneMapping[i].boneIndex;

				if (index >= 0)
				{
					Ogre::String& boneName = *m_BoneNames[index];
					Ogre::Euler euler(m_kinectHelper.m_boneMapping[i].ori);

					transformBone(boneName, euler);
					//eulers[index] = euler;

				}

			}


			//if (m_kinectHelper.m_lastFrameOk)
			{
			
		//		m_kinectHelper.mapBones(m_BoneEulers);

			
			}
		}


		

		/*
		if (m_BoneEulers.size() > 0)
		{
			Ogre::Euler boneEuler = m_BoneEulers[m_boneIndex];
			Ogre::String& boneName = *m_BoneNames[m_boneIndex];


			transformBone(boneName, boneEuler);

		}
		*/
	}

	return true;
};

int CDXView::CDX::OnCreate(LPCREATESTRUCT pcs)
{
	UNREFERENCED_PARAMETER(pcs);

	// Initialize OGre
	if( SUCCEEDED( InitOgre( *this ) ) )
	{
		// Create the scene geometry
     //   if( SUCCEEDED( InitGeometry() ) )
        {
			// Show the window
			ShowWindow(SW_SHOWDEFAULT);
			UpdateWindow();
		}
	}
	else
		TRACE("Failed to initialize DirectX\n");
	

	// initialize kinect 

	if (!m_kinectHelper.initKinect())
	{
	
		TRACE("Failed to initialize Kinect\n");
	}



	return 0;
}

void CDXView::CDX::OnDestroy()
{

	for (int i = 0; i < m_BoneNames.size(); i++)
	{
		delete m_BoneNames[i];
	}

	m_BoneNames.clear();
	m_BoneEulers.clear();

	m_kinectHelper.destroyKinect();


	// End this thread
	::PostQuitMessage(0);
}

LRESULT CDXView::CDX::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int cx = GET_X_LPARAM(lParam);
	int cy = GET_Y_LPARAM(lParam);
	
	SetWindowPos(NULL, 0, 0, cx, cy, SWP_SHOWWINDOW);

	m_RenderWindow->resize(cx, cy);

	return FinalWindowProc(uMsg, wParam, lParam);
}

void CDXView::CDX::PreCreate(CREATESTRUCT &cs)
{
	// An initial window size to allow InitD3D to succeed
	cs.x = 0;
	cs.y = 0;
	cs.cx = 100;
	cs.cy = 100;
}


HRESULT CDXView::CDX::InitResources()
{
	Ogre::Root* root = m_ogreRoot;


	// Load resources
	//Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	Ogre::ConfigFile cf;
	cf.load("resources_d.cfg");
	Ogre::String name, locType;
	Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();

	while (secIt.hasMoreElements())
	{
		Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator it;

		for (it = settings->begin(); it != settings->end(); ++it)
		{
			locType = it->first;
			name = it->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
		}

	}
	Ogre::String sec, type, arch;



	const Ogre::ResourceGroupManager::LocationList genLocs = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList("General");
	arch = genLocs.front()->archive->getName();
	/*
	arch = "../../media";
	*/
	type = "FileSystem";
	sec = "General";

	if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl"))
	{
		if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl150"))
		{
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/materials/programs/GLSL150", type, sec);
		}
		else
		{
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/materials/programs/GLSL", type, sec);
		}

		if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl400"))
		{
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/materials/programs/GLSL400", type, sec);
		}
	}

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	return S_OK;
}

void CDXView::CDX::Render()
// Draws the scene.
{
	if (IsWindow() && m_RenderWindow)
	{
		Ogre::Root* root = m_ogreRoot;
		if (m_First)
		{

			InitResources();
			InitGeometry();

		}
	
		m_First = false;

		if (m_kinectHelper.initialized())
		{
			m_kinectHelper.update();
		}


		root->renderOneFrame();

	

	}
}



LRESULT CDXView::CDX::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	static int _ScreenX=10;
	static int _ScreenY=10;


	switch(uMsg)
	{
		case WM_SIZE:
		{
			_ScreenX = GET_X_LPARAM(lParam);
			_ScreenY = GET_Y_LPARAM(lParam);

			return OnSize(uMsg, wParam, lParam);
		};
		break;
		case WM_LBUTTONDOWN:
		{
			m_mouseDown = true;

		}
		break;
		case WM_MOUSEMOVE:
		{

			if (m_mouseDown &&  m_BoneNames.size() > 0)
			{
				//calculate euler

				float mousePosX = (float)GET_X_LPARAM(lParam)/ (float)_ScreenX;
				float mousePosY = (float)GET_Y_LPARAM(lParam)/ (float)_ScreenY;

				float eulerX = (mousePosX-0.5f) *3.1416;
				float eulerY = (mousePosY-0.5f) *3.1416;
				
				m_BoneEulers[m_boneIndex].setYaw(Ogre::Radian(eulerX));
				m_BoneEulers[m_boneIndex].setPitch(Ogre::Radian(eulerY));
			//	m_BoneEulers[m_boneIndex].setRoll(Ogre::Radian(eulerY));


			}
		}
		break;
		case WM_LBUTTONUP:
		{
			m_mouseDown = false;

		

		}
		break;
		case WM_RBUTTONUP:
		{
			m_mouseDown = false;

			if (m_BoneNames.size() > 0)
			{
				m_boneIndex++;
				m_boneIndex = m_boneIndex % m_BoneNames.size();

			}
			
		
		}

		break;

	}

	return WndProcDefault(uMsg, wParam, lParam);
}


/////////////////////////////////////////////////
// Definitions for the CDXView class
//
CDXView::~CDXView()
{
	// Ensure the DXThread ends before destroying this object.
	::WaitForSingleObject(m_DXThread.GetThread(), INFINITE);
}

int CDXView::OnCreate(LPCREATESTRUCT pcs)
{
	// Create our thread. The thread creates the DX child window when starts
	m_DXThread.CreateThread();
	
	return CWnd::OnCreate(pcs);
}

LRESULT CDXView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_DX.IsWindow())
		m_DX.PostMessage(WM_SIZE, wParam, lParam);

	return FinalWindowProc(uMsg, wParam, lParam);
}

LRESULT CDXView::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_SIZE: return OnSize(uMsg, wParam, lParam);
	}

	return WndProcDefault(uMsg, wParam, lParam);
}

