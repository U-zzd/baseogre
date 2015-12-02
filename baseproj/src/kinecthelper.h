#ifndef KINECT_HELPER
#define KINECT_HELPER

#include "kinectorientfilter.h"

	class KinectHelper 
	{

	public:

		IKinectSensor*          m_pKinectSensor;
		IBodyFrameReader*       m_pBodyFrameReader;
		

		JointsCollection m_boneMapping;
		JointsCollection m_boneMappingFiltered;
		KinectOrientFilter m_orientFilter;


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