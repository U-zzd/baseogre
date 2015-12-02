#ifndef KINECT_ORIENT_H_INCLUDED
#define KINECT_ORIENT_H_INCLUDED



typedef struct _JointRead
{

	Ogre::Quaternion ori;
	Ogre::Vector3 pos;
	int boneIndex;
	TrackingState state;

} JointRead;
 

typedef std::array<JointRead, JointType_Count> JointsCollection;


typedef struct _FilterSmoothParameters
{
	float MaxDeviationRadius;  // Size of the max prediction radius Can snap back to noisy data when too high
	float Smoothing;		// How much soothing will occur.  Will lag when too high
	float Correction;		// How much to correct back from prediction.  Can make things springy
	float Prediction;		// Amount of prediction into the future to use. Can over shoot when too high
	float JitterRadius;     // Size of the radius where jitter is removed. Can do too much smoothing when too high

} FilterSmoothParameters;

class KinectOrientFilter
{
	
	typedef struct _FilterDoubleExponentialData
	{
		
		Ogre::Quaternion RawBoneOrientation;
		Ogre::Quaternion FilteredBoneOrientation;
		Ogre::Quaternion Trend;
		unsigned long FrameCount;
		

	} FilterDoubleExponentialData;




protected:

	std::array<FilterDoubleExponentialData, JointType_Count> m_history;
	

	void filterJoint(JointsCollection& raw, JointType jt, FilterSmoothParameters smoothParams);


public:
	void init();
	void reset();
	void filter(JointsCollection& raw, JointsCollection& filtered);
	FilterSmoothParameters m_smoothParameters;

};


#endif
