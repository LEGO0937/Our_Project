#pragma once
#include "../stdafx.h"

enum RIGID_BODY_STATE { RE = 0 };
enum Body_Type { _dynamic = 1, _static = 2 };
class RigidBody
{
private:
	RIGID_BODY_STATE m_rPrevState = RE;
	Body_Type m_bType = _dynamic;
	float m_fMass = 0.f;
	float m_fMassInverse = 0.0f;

	XMFLOAT3 m_xmf3AcceleratingForce = { 0.f,0.f,0.f };
	float m_fGravity = 9.8f;

	float m_fSpeed = 0.f;
	float m_fMaxSpeed = 0.f;

	XMFLOAT4 m_xmf4Quarternion = { 1.0f,1.0f,1.0f,1.0f };

	XMFLOAT3 m_xmf3Forces = { 0.f,0.f,0.f };
	XMFLOAT3 m_xmf3Torque = { 0.f,0.f,0.f };
	XMFLOAT3 m_xmf3Moments = { 0.f,0.f,0.f };

	XMFLOAT3 m_xmf3Velocity = { 0.f,0.f,0.f };
	XMFLOAT3 m_xmf3AngularVel = { 0.f,0.f,0.f };

	float m_fLinearDamping = 0;
	float m_fAngularDamping = 0;

	XMFLOAT4X4 mUserInertiaTensorLocalInverse = Matrix4x4::Identity();
	XMFLOAT4X4 m_xmf4x4Tensor = Matrix4x4::Identity();   //텐서
	XMFLOAT4X4 m_xmf4x4Tensor_inv = Matrix4x4::Identity();

	XMFLOAT4X4& m_xmf4x4World;

	bool m_bIsSleeping = false;
public:
	void SetMass(const float& mass) { m_fMass = mass; }
	void SetInvMass(const float& mass) { m_fMassInverse = mass; }
	void SetMoment(const XMFLOAT3& moment) { m_xmf3Moments = moment; }
	void SetAcceleratingForce(const XMFLOAT3& acc) { m_xmf3AcceleratingForce = acc; }
	void SetGravity(const float& gravity) { m_fGravity = gravity; }

	void SetSpeed(const float& speed) { m_fSpeed = speed; }
	void SetMaxSpeed(const float& maxSpeed) { m_fMaxSpeed = maxSpeed; }

	void SetForce(const XMFLOAT3& force) { m_xmf3Forces = force; }

	void SetVelocity(const XMFLOAT3& velocity) { m_xmf3Velocity = velocity; }
	void SetAngularVel(const XMFLOAT3& angularVel) { m_xmf3AngularVel = angularVel; }
	void SetLinearDamping(const float& damping) { m_fLinearDamping = damping; }
	void SetAngularDamping(const float& damping) { m_fAngularDamping = damping; }

	void SetInertiaTensor(const XMFLOAT4X4& tensor) { m_xmf4x4Tensor = tensor; }
	void SetInverseInertiaTensor(const XMFLOAT4X4& tensor_inv) { m_xmf4x4Tensor_inv = tensor_inv; }

	void SetIsSleeping(const bool& isSleeping) {

		if (isSleeping) {
			m_xmf3Velocity = XMFLOAT3(0, 0, 0);
			m_xmf3AngularVel = XMFLOAT3(0, 0, 0);
			m_xmf3Forces = XMFLOAT3(0, 0, 0);
			m_xmf3Torque = XMFLOAT3(0, 0, 0);
		}

		m_bIsSleeping = isSleeping;
	}
	void SetPosition(const XMFLOAT3& pos)
	{
		m_xmf4x4World._41 = pos.x;
		m_xmf4x4World._42 = pos.y;
		m_xmf4x4World._43 = pos.z;
	}
	void SetQuarternion(const XMFLOAT4& quar)
	{
		m_xmf4Quarternion = quar;
	}
	float GetMass() { return m_fMass; }
	float GetIneMass() { return m_fMassInverse; }

	XMFLOAT3 GetMoment() { return m_xmf3Moments; }
	XMFLOAT3 GetAcceleratingForce() { return m_xmf3AcceleratingForce; }
	float GetGravity() { return m_fGravity; }
	float GetSpeed() { return m_fSpeed; }
	float GetMaxSpeed() { return m_fMaxSpeed; }

	XMFLOAT3 GetForce() { return m_xmf3Forces; }

	XMFLOAT3 GetVelocity() { return m_xmf3Velocity; }
	XMFLOAT3 GetAngularVel() { return m_xmf3AngularVel; }
	float GetLinearDamping() { return m_fLinearDamping; }
	float GetAngularDamping() { return m_fAngularDamping; }

	XMFLOAT4X4 GetInertiaTensor() { return m_xmf4x4Tensor; }
	XMFLOAT4X4 GetInverseInertiaTensor() { return m_xmf4x4Tensor_inv; }

	XMFLOAT3 GetPosition() { return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43); }
	XMFLOAT4 GetQuarternion() { return m_xmf4Quarternion; }
	/// Update the transform of the body after a change of the center of mass

	/// Update the broad-phase state for this body (because it has moved for instance)
	void updateBroadPhaseState();

	/// Update the world inverse inertia tensor of the body
	void updateInertiaTensorInverseWorld();

	void applyForceToCenterOfMass(const XMFLOAT3 & force) {

		// 다이나믹 바디가 아니라면 아무것도하지 않습니다
		if (m_bType != Body_Type::_dynamic) return;

		// 자고 있다면 몸을 깨운다
		if (m_bIsSleeping) {
			SetIsSleeping(false);
		}

		// 힘을 추가
		m_xmf3Forces = Vector3::Add(m_xmf3Forces, force);
	}

	void applyForce(const XMFLOAT3& force, const XMFLOAT3& point) {

		// If it is not a dynamic body, we do nothing
		if (m_bType != Body_Type::_dynamic) return;

		// Awake the body if it was sleeping
		if (m_bIsSleeping) {
			SetIsSleeping(false);
		}

		// Add the force and torque
		m_xmf3Forces = Vector3::Add(m_xmf3Forces, force);
		XMFLOAT3 v = Vector3::Subtract(point, XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
		m_xmf3Torque = Vector3::CrossProduct(v, force, false);
	}

	void applyTorque(const XMFLOAT3& torque) {

		// If it is not a dynamic body, we do nothing
		if (m_bType != Body_Type::_dynamic) return;

		// Awake the body if it was sleeping
		if (m_bIsSleeping) {
			SetIsSleeping(false);
		}

		// Add the torque
		m_xmf3Torque = Vector3::Add(m_xmf3Torque, torque);
	}

	void updateTransformWithCenterOfMass() {

		// Translate the body according to the translation of the center of mass position
		//mTransform.setPosition(mCenterOfMassWorld - mTransform.getOrientation() * mCenterOfMassLocal);
	}

};