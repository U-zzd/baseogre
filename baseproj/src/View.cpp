//////////////////////////////////////////////
// View.cpp

// Includes backbuffer resizing suggested by Guillaume Werlé

// NOTE: for MS compilers you will need the DirectX SDK v9
//       for Dev-C++ you will need the DirecX v9.0c DevPak


#include "stdafx.h"
#include "DXApp.h"
#include <OgreConfigFile.h>
#include <OgreGLRenderSystem.h>

#define USEKINECT (1)


enum ModesVisualBones
{
	VisualSceneNode1 = 0,
	VisualSceneNode2,
	VisualSceneNode3,
	VisualSceneNode4,
	VisualSceneNode5,
	bonefootl,
	VisualSceneNode8,
	VisualSceneNode9,
	bonefootr,
	VisualSceneNode12,
	VisualSceneNode14,
	VisualSceneNode15,
	VisualSceneNode36,
	VisualSceneNode17,
	bonendingl,
	VisualSceneNode37,
	VisualSceneNode38,
	VisualSceneNode39,
	VisualSceneNode40,
	VisualSceneNode41,
	VisualSceneNode62,
	VisualSceneNode43,
	boneendingr,
	boneCount
};


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
	while (Msg.message != WM_QUIT)
	{
		if (PeekMessage(&Msg, NULL, 0U, 0U, PM_REMOVE))
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
	m_centerDown = false;

	m_boneIndex = 0;
	m_entity = NULL;

}

CDXView::CDX::~CDX()
{

	m_Camera = NULL;
	m_RenderWindow = NULL;


}

HRESULT CDXView::CDX::InitOgre(HWND hWnd)
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

	parms["externalWindowHandle"] = s;
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
	m_Camera->setPosition(Ogre::Vector3(0.0, 2.0, 5.0));

	Ogre::SceneNode *CameraNode = NULL;
	CameraNode = SceneManager->getRootSceneNode()->createChildSceneNode("CameraNode");


	Ogre::Viewport* Viewport = NULL;

	if (0 == m_RenderWindow->getNumViewports()){
		Viewport = m_RenderWindow->addViewport(m_Camera);
		Viewport->setBackgroundColour(Ogre::ColourValue(0.8f, 1.0f,
			0.8f));
	}

	m_Camera->setAspectRatio(Ogre::Real(rect.Width()) /
		Ogre::Real(rect.Height()));

	new DebugDrawer(SceneManager, 0.5f);


	return S_OK;
}

HRESULT CDXView::CDX::InitGeometry()
// Creates the scene geometry.
{
	//load the ogre robot


	Ogre::SceneManager *SceneManager = m_Camera->getSceneManager();



	//	m_entity = SceneManager->createEntity("Robot", "robot.mesh");
	m_entity = SceneManager->createEntity("Robot", "Geometry1.mesh");


	Ogre::SceneNode *RobotNode = SceneManager->getRootSceneNode()->createChildSceneNode();
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



void CDXView::CDX::setupAnimations()
{
	// this is very important due to the nature of the exported animations
	//entidadPersonaje->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);	

	Ogre::String bones[] =
	{
		"VisualSceneNode1",
		"VisualSceneNode2",
		"VisualSceneNode3",
		"VisualSceneNode4",
		"VisualSceneNode5",
		"bonefootl",
		"VisualSceneNode8",
		"VisualSceneNode9",
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







	m_kinectHelper.setBoneMapping(JointType_SpineMid, VisualSceneNode12);
	m_kinectHelper.setBoneMapping(JointType_ElbowRight, VisualSceneNode41);
	//m_kinectHelper.setBoneMapping(JointType_WristRight, VisualSceneNode43);
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
		m_BoneEulers.push_back(Ogre::Euler());
		m_BoneQuats.push_back(Ogre::Quaternion::IDENTITY);
		m_BoneTracked.push_back(false);
	}

	//m_BoneTracked[VisualSceneNode15] = true;
	//m_BoneTracked[VisualSceneNode36] = true;
	//m_BoneTracked[VisualSceneNode17] = true;
	m_BoneTracked[VisualSceneNode41] = true;
	//m_BoneTracked[VisualSceneNode62] = true;
	//m_BoneTracked[VisualSceneNode43] = true;
	m_BoneTracked[VisualSceneNode12] = true;



	Ogre::AnimationStateSet* animations = m_entity->getAllAnimationStates();
	Ogre::AnimationStateIterator it = animations->getAnimationStateIterator();

	//set all to manualy controlled
	Ogre::Quaternion ki = Ogre::Quaternion::IDENTITY;
	Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;

	for (int i = 0; i < m_BoneNames.size(); i++)
	{
		Ogre::String& boneName = *m_BoneNames[i];
		setupBone2(boneName, q);
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


Ogre::Quaternion CDXView::CDX::calculateBoneTransform(const Ogre::String& modelBoneName, Ogre::Quaternion& q)
{

	Ogre::Skeleton* skel = m_entity->getSkeleton();
	Ogre::Bone* bone = skel->getBone(modelBoneName);
	Ogre::Quaternion qI = bone->getInitialOrientation();
	Ogre::Quaternion newQ = Ogre::Quaternion::IDENTITY;

	newQ = q * qI;

	return newQ;

}

void  CDXView::CDX::applyBoneTransform(const Ogre::String& modelBoneName, Ogre::Quaternion& q)
{
	Ogre::Skeleton* skel = m_entity->getSkeleton();
	Ogre::Bone* bone = skel->getBone(modelBoneName);
	bone->setOrientation(q);

}



bool CDXView::CDX::frameStarted(const Ogre::FrameEvent& evt)
{

	if (m_entity && !m_First)
	{

		if (m_BoneEulers.size() > 0)
		{

			for (int i = 0; i < m_BoneEulers.size(); i++)
			{
				if (m_BoneTracked[i])
				{

					Ogre::String& boneName = *m_BoneNames[i];
					Ogre::Skeleton* skel = m_entity->getSkeleton();
					Ogre::Bone* bone = skel->getBone(boneName);

					Ogre::Quaternion qI = bone->getInitialOrientation();
					Ogre::Quaternion qC = bone->getOrientation();




#if (USEKINECT)
					///get orient kinect	
					Ogre::Quaternion kinectOri = Ogre::Quaternion::IDENTITY;
					Ogre::Quaternion adjusted = kinectOri;

					if (m_kinectHelper.initialized())
					{

						for (int j = 0; j < m_kinectHelper.m_boneMapping.size(); j++)
						{
							if (i == m_kinectHelper.m_boneMapping[j].boneIndex)
							{

								kinectOri = m_kinectHelper.m_boneMapping[j].ori;

								adjusted = kinectOri;

								//rotate 180 around y (camera space)
								Ogre::Quaternion yrot;
								yrot.FromAngleAxis(Ogre::Degree(180.0), Ogre::Vector3(0.0, 1.0, 0.0));
								
								adjusted = yrot*adjusted;

							
								if (j == JointType_ElbowRight)
								{
									Ogre::Vector3 ax;
									Ogre::Vector3 ay;
									Ogre::Vector3 az;
									Ogre::Vector3 ax2;
									Ogre::Vector3 ay2;
									Ogre::Vector3 az2;									
									Ogre::Quaternion base;
									
									adjusted.ToAxes(ax, ay, az);																		
									ax2 = ax;
									ay2 = az;
									az2 = ay*-1;
									adjusted.FromAxes(ax2, ay2, az2);
									ax = Ogre::Vector3(0.0, 0.0, -1.0);
									ay = Ogre::Vector3(-1.0, 0.0, 0.0);
									az = Ogre::Vector3(0.0, 1.0, 0.0);
									base.FromAxes(ax, ay, az);
									adjusted = base.Inverse()*  adjusted;

									
								}



							

							}
						}
					}

					


#else
					//get manual orien 
					Ogre::Quaternion kinectOri = m_BoneEulers[i].toQuaternion();
					Ogre::Quaternion adjusted = kinectOri;
#endif

					
					Ogre::Quaternion finalquat = calculateBoneTransform(boneName, adjusted);
					m_BoneQuats[i] = finalquat;


					Ogre::Quaternion debugQuat = finalquat;



					//draw debug lines

					Ogre::Vector3 bonePos = bone->convertLocalToWorldPosition(Ogre::Vector3(0.0, 0.0, 0.0));
					Ogre::Vector3 ax;
					Ogre::Vector3 ay;
					Ogre::Vector3 az;
					debugQuat.ToAxes(ax, ay, az);
					ax.normalise();
					ay.normalise();
					az.normalise();

					bool xback = ax.z < 0;
					bool yback = ay.z < 0;
					bool zback = az.z < 0;

					bonePos = bonePos + Ogre::Vector3(0.0, 0.0, 0.5);
					ax = bonePos + ax*0.2f;
					ay = bonePos + ay*0.2f;
					az = bonePos + az*0.2f;

					
					Ogre::ColourValue clr = Ogre::ColourValue::Red;
					if (xback)
					{
						clr.r = clr.r* 0.6;
						clr.g = clr.g* 0.6;
						clr.b = clr.b* 0.6;
					}

					DebugDrawer::getSingleton().drawLine(bonePos, ax, clr);

					clr = Ogre::ColourValue::Green;
					if (yback)
					{
						clr.r = clr.r* 0.6;
						clr.g = clr.g* 0.6;
						clr.b = clr.b* 0.6;
					}
					DebugDrawer::getSingleton().drawLine(bonePos, ay, clr);

					clr = Ogre::ColourValue::White;
					if (zback)
					{
						clr.r = clr.r* 0.6;
						clr.g = clr.g* 0.6;
						clr.b = clr.b* 0.6;
					}

					DebugDrawer::getSingleton().drawLine(bonePos, az, clr);


				}
			}
		}


	}

	m_entity->setVisible(true);

	DebugDrawer::getSingleton().build();
	return true;
}

bool CDXView::CDX::frameEnded(const Ogre::FrameEvent& evt)
{
	DebugDrawer::getSingleton().clear();
	return true;
}

bool CDXView::CDX::frameRenderingQueued(const Ogre::FrameEvent& evt)
{

	/*if (mAnimationState)
	mAnimationState->addTime(evt.timeSinceLastFrame);
	*/

	if (m_entity && !m_First)
	{

		if (m_BoneEulers.size() > 0)
		{

			for (int i = 0; i < m_BoneEulers.size(); i++)
			{
				if (m_BoneTracked[i])
				{

					Ogre::Quaternion quat = m_BoneQuats[i];
					Ogre::String& boneName = *m_BoneNames[i];
					applyBoneTransform(boneName, quat);
				};

			}
		}

	}

	return true;
};

int CDXView::CDX::OnCreate(LPCREATESTRUCT pcs)
{
	UNREFERENCED_PARAMETER(pcs);

	// Initialize OGre
	if (SUCCEEDED(InitOgre(*this)))
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


	InitResources();

	// initialize kinect 

#if USEKINECT
	if (!m_kinectHelper.initKinect())
	{

		TRACE("Failed to initialize Kinect\n");
	}
#endif


	return 0;
}

void CDXView::CDX::OnDestroy()
{

	delete DebugDrawer::getSingletonPtr();

	for (int i = 0; i < m_BoneNames.size(); i++)
	{
		delete m_BoneNames[i];
	}

	m_BoneNames.clear();
	m_BoneEulers.clear();
	m_BoneQuats.clear();

#if USEKINECT
	m_kinectHelper.destroyKinect();
#endif


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

			m_RenderWindow->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);


			InitGeometry();

		}

		m_First = false;

#if USEKINECT
		if (m_kinectHelper.initialized())
		{
			m_kinectHelper.update();
		}
#endif


		root->renderOneFrame();



	}
}



LRESULT CDXView::CDX::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	static int _ScreenX = 10;
	static int _ScreenY = 10;


	switch (uMsg)
	{
	case WM_SIZE:
	{
		_ScreenX = GET_X_LPARAM(lParam);
		_ScreenY = GET_Y_LPARAM(lParam);

		return OnSize(uMsg, wParam, lParam);
	};
	break;
	case WM_MBUTTONDOWN:
	{

		m_centerDown = true;
	}
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

			float mousePosX = (float)GET_X_LPARAM(lParam) / (float)_ScreenX;
			float mousePosY = (float)GET_Y_LPARAM(lParam) / (float)_ScreenY;

			float eulerX = (mousePosX - 0.5f) *3.1416;
			float eulerY = (mousePosY - 0.5f) *3.1416;

			m_BoneEulers[m_boneIndex].setYaw(Ogre::Radian(eulerX));
			m_BoneEulers[m_boneIndex].setPitch(Ogre::Radian(eulerY));
			//	m_BoneEulers[m_boneIndex].setRoll(Ogre::Radian(eulerY));



		}
		else if (m_centerDown)
		{


			float mousePosX = (float)GET_X_LPARAM(lParam) / (float)_ScreenX;
			float mousePosY = (float)GET_Y_LPARAM(lParam) / (float)_ScreenY;
			float teta1 = (mousePosX - 0.5f) *3.1416;
			float teta2 = (mousePosY - 0.5f) *3.1416;
			float radius = 5.0;


			teta1 = -teta1;

			Ogre::Vector3 w(Ogre::Math::Sin(teta1), 0.0f, Ogre::Math::Cos(teta1));
			Ogre::Vector3 y(0.0f, Ogre::Math::Sin(teta2), 0.0f);

			w = w * radius;
			y = y * radius;
			w = w * Ogre::Math::Cos(teta2);

			y = y + w + Ogre::Vector3(0.0, 2.0, 0.0);

			m_Camera->setPosition(y);

			Ogre::AxisAlignedBox Box = m_entity->getBoundingBox();
			Ogre::Vector3 Center = Box.getCenter();
			m_Camera->lookAt(Center);


		}
	}
	break;
	case WM_MBUTTONUP:
	{
		m_mouseDown = false;
		m_centerDown = false;
	}
	break;
	case WM_LBUTTONUP:
	{
		m_mouseDown = false;
		m_centerDown = false;


	}
	break;
	case WM_RBUTTONUP:
	{
		m_mouseDown = false;
		m_centerDown = false;

		if (m_BoneNames.size() > 0)
		{
			do
			{
				m_boneIndex++;
				m_boneIndex = m_boneIndex % m_BoneNames.size();

				if (m_BoneTracked[m_boneIndex])
					break;

			} while (true);
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
	switch (uMsg)
	{
	case WM_SIZE: return OnSize(uMsg, wParam, lParam);
	}

	return WndProcDefault(uMsg, wParam, lParam);
}

