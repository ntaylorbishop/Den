#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//KEYFRAME
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
template <typename T>
struct Keyframe {
	float m_parametric;
	T m_value;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//KEYFRAMEPROPERTY
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
template <typename T>
class KeyFrameProperty {

	void AddKeyframe(float parametricKey, const T& value);
	const T EvaluateAtParametric(float parametricKey); //This depends on clamping or wrapping based on what the KeyFramedAnim says

	std::vector<Keyframe<T>> m_keyFrames;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//KEYFRAMEDANIM
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
template <typename T>
class KeyFramedAnim {

	T Evaluate();
	void Update(float deltaSeconds);

	float m_durationSeconds;
	float m_currentSeconds;
	KeyFrameProperty<T> m_keyframedProperties;
};

