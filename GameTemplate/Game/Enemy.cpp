#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"
#include "Game.h"

Enemy::Enemy()
{
}


Enemy::~Enemy()
{
	DeleteGO(m_skinModelRender);
}

bool Enemy::Start()
{
	m_player = FindGO<Player>("player");
	m_game = FindGO<Game>("Game");

	m_skinModelRender = NewGO<prefab::CSkinModelRender>(0);
	m_skinModelRender->Init(
		L"modelData/enemy00.cmo"
	);
	m_skinModelRender->SetPosition(m_position);
	m_skinModelRender->SetRotation(m_rotation);
	m_skinModelRender->SetScale(m_scale);

	m_physicsStaticObject.CreateMesh(m_position, m_rotation, m_scale, m_skinModelRender);
	m_startPos = m_position;
	return true;
}
void Enemy::Update()
{
	CVector3 v = m_player->GetPosition() - m_position;
	float len = v.Length();
	v.Normalize();
	CVector3 up = { 0.0f,1.0f,0.0f };
	float naiseki = v.Dot(up);
	float angle = acos(naiseki);
	//距離が５０より小さいとき
	//攻撃
	if (fabs(angle) > CMath::DegToRad(70) 
		&& len < 70 
		&& m_player->GetJumpFlag() == false) {
		m_game->SetPlayerHp(-1);
		CVector3 force = v;
		//Y成分を除去。
		force.y = 0.0f;
		force.Normalize();
		force *= 2000;
		m_player->SetSpeed(force);
	}
	//距離が50より大きい500より小さいとき
	//追跡
	if (len > 50 && len < 400) {
		m_position += v * 3;
		m_position.y = 0.0f;
	}
	if (len > 400) {
		auto m_posV = m_startPos - m_position;
		m_posV.Normalize();
		m_position += m_posV * 5.0f;
	}
	m_skinModelRender->SetPosition(m_position);
	m_physicsStaticObject.SetPositionAndRotation(m_position, CQuaternion::Identity);

	//ジャンプ中かつ距離が５０より小さい
	if (m_player->GetJumpFlag() == true && len < 90) {
		effect = NewGO<prefab::CEffect>(0);
		effect->Play(L"effect/star.efk");
		effect->SetPosition(m_position);
		effect->SetScale({ 10.0f,10.0f,10.0f });
		//サウンド再生
		m_sound = NewGO <prefab::CSoundSource>(0);
		m_sound->Init(L"sound/enemyDown.wav");
		m_sound->Play(false);
		m_game->SetScore(20);
		DeleteGO(this);
	}
}