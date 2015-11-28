#ifndef KINECT_HELPER
#define KINECT_HELPER


	class KinectHelper 
	{

	public:

		IKinectSensor*          m_pKinectSensor;
		IBodyFrameReader*       m_pBodyFrameReader;


		typedef struct _JointRead
		{

			Ogre::Quaternion ori;
			Ogre::Vector3 pos;
			int boneIndex;
		

		} JointRead;

		std::array<JointRead, JointType_Count> m_boneMapping;


		bool m_initialized;
		bool m_lastFrameOk;
		/// Default constructor.
		KinectHelper() : m_pKinectSensor(NULL), m_pBodyFrameReader(NULL), m_initialized (false)
		{
		
		};
		
		bool initKinect();
		bool initialized() { return m_initialized; };

		bool destroyKinect();
		
		void setBoneMapping(JointType jointType, int boneIndex);
		void mapBones(std::vector<Ogre::Euler>& eulers);


		void update();
		void processBody(int nBodyCount, IBody** ppBodies);


	};


#endif