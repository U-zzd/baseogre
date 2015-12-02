#include "stdafx.h"
#include "DXApp.h"

float QuaternionAngle(Ogre::Quaternion rotation)
{
	rotation.normalise();

	Ogre::Radian angleR = Ogre::Math::ACos(rotation.w);

	float angle = angleR.valueRadians()*2.0f;
	
	return angle;
}



Ogre::Quaternion EnsureQuaternionNeighborhood(Ogre::Quaternion quaternionA, Ogre::Quaternion quaternionB)
{

	
	if (quaternionA.Dot(quaternionB) < 0)
	{
		// Negate the second quaternion, to place it in the opposite 3D sphere.
		return -1*quaternionB;
	}
	
	return quaternionB;

}
Ogre::Quaternion EnhancedQuaternionSlerp(Ogre::Quaternion quaternionA, Ogre::Quaternion quaternionB, float amount)
{
	Ogre::Quaternion modifiedB = EnsureQuaternionNeighborhood(quaternionA, quaternionB);

	Ogre::Quaternion slerp= Ogre::Quaternion::Slerp(amount, quaternionA, quaternionB);

	return slerp;
}

Ogre::Quaternion RotationBetweenQuaternions(Ogre::Quaternion quaternionA, Ogre::Quaternion quaternionB)
{
	Ogre::Quaternion modifiedB = EnsureQuaternionNeighborhood(quaternionA, quaternionB);
	Ogre::Quaternion dif = quaternionA.Inverse() * quaternionB;

	return dif;

}

void KinectOrientFilter::init()
{

}

/*
void KinectOrientFilter::filterJoint(JointsCollection& raw, JointType jt, FilterSmoothParameters smoothParams)
{
	int jointIndex = (int)jt;

	Ogre::Quaternion filteredOrientation;
	Ogre::Quaternion trend;
	Ogre::Quaternion rawOrientation = raw[jointIndex].ori;

	Ogre::Quaternion prevFilteredOrientation = m_history[jointIndex].FilteredBoneOrientation;
	Ogre::Quaternion prevTrend = m_history[jointIndex].Trend;

	bool orientationIsValid = (raw[jointIndex].state == TrackingState_Tracked || raw[jointIndex].state == TrackingState_Inferred);


	if (!orientationIsValid)
	{
		if (m_history[jointIndex].FrameCount > 0)
		{
			rawOrientation = m_history[jointIndex].FilteredBoneOrientation;
			m_history[jointIndex].FrameCount = 0;
		}
	}

	// Initial start values or reset values
	if (m_history[jointIndex].FrameCount == 0)
	{
		// Use raw position and zero trend for first value
		filteredOrientation = rawOrientation;
		trend = Ogre::Quaternion::IDENTITY;
	}
	else
	{
		filteredOrientation = EnhancedQuaternionSlerp(prevFilteredOrientation, rawOrientation, 0.5);
	}
	m_history[jointIndex].RawBoneOrientation = rawOrientation;
	m_history[jointIndex].FilteredBoneOrientation = filteredOrientation;
	m_history[jointIndex].Trend = trend;
	m_history[jointIndex].FrameCount++;

}
*/

void KinectOrientFilter::filterJoint(JointsCollection& raw, JointType jt, FilterSmoothParameters smoothParams)
{
	
	int jointIndex = (int)jt;

	Ogre::Quaternion filteredOrientation;
	Ogre::Quaternion trend;
	Ogre::Quaternion rawOrientation = raw[jointIndex].ori;

	Ogre::Quaternion prevFilteredOrientation = m_history[jointIndex].FilteredBoneOrientation;
	Ogre::Quaternion prevTrend = m_history[jointIndex].Trend;

	bool orientationIsValid =  (raw[jointIndex].state == TrackingState_Tracked || raw[jointIndex].state == TrackingState_Inferred);
	

	if (!orientationIsValid)
	{
		if (m_history[jointIndex].FrameCount > 0)
		{
			rawOrientation = m_history[jointIndex].FilteredBoneOrientation;
			m_history[jointIndex].FrameCount = 0;
		}
	}

	// Initial start values or reset values
	if (m_history[jointIndex].FrameCount == 0)
	{
		// Use raw position and zero trend for first value
		filteredOrientation = rawOrientation;
		trend = Ogre::Quaternion::IDENTITY;
	}
	else if (m_history[jointIndex].FrameCount == 1)
	{
		// Use average of two positions and calculate proper trend for end value
		Ogre::Quaternion prevRawOrientation = m_history[jointIndex].RawBoneOrientation;
		filteredOrientation = EnhancedQuaternionSlerp(prevRawOrientation, rawOrientation, 0.5f);

		Ogre::Quaternion diffStarted = RotationBetweenQuaternions(filteredOrientation, prevFilteredOrientation);
		trend = EnhancedQuaternionSlerp(prevTrend, diffStarted, smoothParams.Correction);
	}
	else
	{
		// First apply a jitter filter
		Ogre::Quaternion diffJitter = RotationBetweenQuaternions(rawOrientation, prevFilteredOrientation);

		
		
		
		float diffValJitter = fabsf(QuaternionAngle(diffJitter));
		if (diffValJitter <= smoothParams.JitterRadius)
		{
			filteredOrientation = EnhancedQuaternionSlerp(prevFilteredOrientation, rawOrientation, diffValJitter / smoothParams.JitterRadius);
		}
		
		else
		{
			filteredOrientation = rawOrientation;
		}
		

		// Now the double exponential smoothing filter
		filteredOrientation = EnhancedQuaternionSlerp(filteredOrientation, prevFilteredOrientation * prevTrend, smoothParams.Smoothing);

		//diffJitter = RotationBetweenQuaternions(filteredOrientation, prevFilteredOrientation);
		trend = EnhancedQuaternionSlerp(prevTrend, diffJitter, smoothParams.Correction);
	}

	// Use the trend and predict into the future to reduce latency
	Ogre::Quaternion  predictedOrientation = filteredOrientation * EnhancedQuaternionSlerp(Ogre::Quaternion::IDENTITY, trend, smoothParams.Prediction);

	// Check that we are not too far away from raw data
	Ogre::Quaternion  diff = RotationBetweenQuaternions(predictedOrientation, filteredOrientation);
	float diffVal = fabsf(QuaternionAngle(diff));

	if (diffVal > smoothParams.MaxDeviationRadius)
	{
		predictedOrientation = EnhancedQuaternionSlerp(filteredOrientation, predictedOrientation, smoothParams.MaxDeviationRadius / diffVal);
	}

	predictedOrientation.normalise();
	filteredOrientation.normalise();
	trend.normalise();

	// Save the data from this frame
	m_history[jointIndex].RawBoneOrientation = rawOrientation;
	m_history[jointIndex].FilteredBoneOrientation = filteredOrientation;
	m_history[jointIndex].Trend = trend;
	m_history[jointIndex].FrameCount++;


}


void KinectOrientFilter::reset()
{
	//Array jointTypeValues = Enum.GetValues(typeof(JointType));
	//m_history = new FilterDoubleExponentialData[jointTypeValues.Length];

	for (int i = 0; i < JointType_Count; i++)
	{
		m_history[i].FilteredBoneOrientation = Ogre::Quaternion::IDENTITY;
		m_history[i].RawBoneOrientation = Ogre::Quaternion::IDENTITY;
		m_history[i].Trend = Ogre::Quaternion::IDENTITY;
		m_history[i].FrameCount = 0;

	}
}

void KinectOrientFilter::filter(JointsCollection& raw, JointsCollection& filtered)
{

	

	FilterSmoothParameters tempSmoothingParams;

	// Check for divide by zero. Use an epsilon of a 10th of a millimeter
	m_smoothParameters.JitterRadius = std::max<float>(0.0001f, m_smoothParameters.JitterRadius);

	
	tempSmoothingParams.Smoothing = m_smoothParameters.Smoothing;
	tempSmoothingParams.Correction = m_smoothParameters.Correction;
	tempSmoothingParams.Prediction = m_smoothParameters.Prediction;

	for (int i = 0; i < JointType_Count; i++)
	{
		/*// If not tracked, we smooth a bit more by using a bigger jitter radius
		// Always filter feet highly as they are so noisy
		if (skeleton.Joints[jt].TrackingState != JointTrackingState.Tracked || jt == JointType.FootLeft || jt == JointType.FootRight)
		{
			tempSmoothingParams.JitterRadius *= 2.0f;
			tempSmoothingParams.MaxDeviationRadius *= 2.0f;
		}
		elsr*/
		{

			tempSmoothingParams.JitterRadius = m_smoothParameters.JitterRadius;
			tempSmoothingParams.MaxDeviationRadius = m_smoothParameters.MaxDeviationRadius;
		}


		
		filterJoint(raw, (JointType)i, tempSmoothingParams);

		filtered[i].boneIndex = raw[i].boneIndex;
		filtered[i].pos = raw[i].pos;
		filtered[i].state = raw[i].state;
		filtered[i].ori = m_history[i].FilteredBoneOrientation;

	}

}


