#include <Novice.h>
#include <cmath>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <imgui.h>
#include <algorithm>
const char kWindowTitle[] = "LD2A_04_トヨダヤヤ_MT3_04_04";

//================================================================
// 構造体の宣言
//================================================================

struct Matrix4x4
{
	float m[4][4];
};

struct Vector3
{
	float x;
	float y;
	float z;
};

struct Sphere
{
	Vector3 center;
	float radius;
};

struct Ball
{
	Vector3 position; // 位置
	Vector3 velocity; // 速度
	Vector3 acceleration; // 加速度
	float mass; // 質量
	float radius; // 半径
	unsigned int color; // 色
};

struct Plane
{
	Vector3 normal;
	float distance;
};

struct Segment
{
	Vector3 origin;
	Vector3 diff;
};

struct Capsule
{
	Segment segment;
	float radius;
};

//================================================================
// 関数の宣言
//================================================================

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// 三次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// 減算関数
Vector3 Subtract(const Vector3& a, const Vector3& b);

// 加算関数
Vector3 Add(const Vector3& v1, const Vector3& v2);

// 割り算関数
Vector3 Divide(const Vector3& v, const float& scalar);

// 積(ベクトル)
Vector3 Vector3Multiply(const float& v1, const Vector3& v2);

// ノルン(長さ)の関数
float Length(const Vector3& v);

// 正規化の関数
Vector3 Normalize(const Vector3& v);

// 内積の関数
float Dot(const Vector3& v1, const Vector3& v2);

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

// 法線と垂直なベクトルを求める
Vector3 Perpendicular(const Vector3& vector);

// グリットの描画
void DrawGrid(const Matrix4x4& worldViewProjectionMatrix, const Matrix4x4& viewportMatrix);

// 平面の描画
void DrawPlane(const Plane& plane, const Matrix4x4& worldViewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// 球の描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& worldViewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// 当たり判定
bool isCollision(const Ball& ball, const Plane& plane);

// 正射影ベクトルの関数
Vector3 Project(const Vector3& v1, const Vector3& v2);

// 反射ベクトルを求める関数
Vector3 Reflect(const Vector3& input, const Vector3& normal);

// 反発関数
Vector3 Bounce(Ball& ball, const Plane& plane,const float deltaTime);

// カプセル判定
bool IsCollisionCapsule(const Ball& ball,const Plane& plane,const float deltaTime);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//================================================================
	// 変数の宣言と初期化
	//================================================================

	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;

	// カメラの位置と角度
	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	// ボール
	Ball ball{};
	ball.position = { 0.8f,1.2f,0.3f };
	ball.acceleration = { 0.0f,-9.8f,0.0f };
	ball.velocity = { 0.0f,0.0f,0.0f };
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = WHITE;
	bool isStart = false;

	// 平面
	Plane plane;
	plane.normal = Normalize({ -0.2f,0.9f,-0.3f });
	plane.distance = 0.0f;

	// デルタタイム
	float deltaTime = 1.0f / 60.0f;

	// 描画用変数
	Sphere sphere{};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		//================================================================
		// グリットの計算処理
		//================================================================

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		//================================================================
		// 反発の計算処理
		//================================================================

		if (isStart)
		{
			ball.position = Bounce(ball, plane, deltaTime);
		}

		sphere.center = ball.position;
		sphere.radius = ball.radius;

		//================================================================
		// ImGuiの処理
		//================================================================

		ImGui::Begin("Window");
		ImGui::Checkbox("isStart", &isStart);
		ImGui::Text("Ball Position: (%.2f, %.2f, %.2f)", ball.position.x, ball.position.y, ball.position.z);
		ImGui::End();

		
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//================================================================
		// 描画処理
		//================================================================

		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		DrawPlane(plane, worldViewProjectionMatrix, viewportMatrix, WHITE);
		DrawSphere(sphere, worldViewProjectionMatrix, viewportMatrix, ball.color);
		

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

void DrawGrid(const Matrix4x4& worldViewProjectionMatrix, const Matrix4x4& viewportMatrix)
{
	const float kGridHalfWidth = 2.0f; // gridの半分の幅
	const uint32_t kSubdivision = 10; // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 一つ分の長さ

	// 奥から手前への線を順番に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex)
	{
		float x = -kGridHalfWidth + kGridEvery * float(xIndex);
		Vector3 start = { x, 0.0f, -kGridHalfWidth };
		Vector3 end = { x, 0.0f, kGridHalfWidth };
		Vector3 startNdc = Transform(start, worldViewProjectionMatrix);
		Vector3 startScreen = Transform(startNdc, viewportMatrix);
		Vector3 endNdc = Transform(end, worldViewProjectionMatrix);
		Vector3 endScreen = Transform(endNdc, viewportMatrix);
		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), 0xFFFFFFFF);
	}

	// 左から右への線を順番に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex)
	{
		float z = -kGridHalfWidth + kGridEvery * float(zIndex);

		Vector3 start = { -kGridHalfWidth, 0.0f, z };
		Vector3 end = { kGridHalfWidth, 0.0f, z };
		Vector3 startNdc = Transform(start, worldViewProjectionMatrix);
		Vector3 startScreen = Transform(startNdc, viewportMatrix);
		Vector3 endNdc = Transform(end, worldViewProjectionMatrix);
		Vector3 endScreen = Transform(endNdc, viewportMatrix);
		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), 0xFFFFFFFF);
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& worldViewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	const uint32_t kSubdivision = 10;
	const float kLonEvery = (2.0f * float(M_PI)) / float(kSubdivision);
	const float kLatEvery = float(M_PI) / float(kSubdivision);
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		float lat = float(-M_PI) / 2.0f + kLatEvery * float(latIndex);

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			float lon = lonIndex * kLonEvery;
			Vector3 a, b, c;
			a = {
				sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon),
				sphere.center.y + sphere.radius * std::sin(lat),
				sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon) };
			b = {
				sphere.center.x + sphere.radius * std::cos(lat + kLatEvery) * std::cos(lon),
				sphere.center.y + sphere.radius * std::sin(lat + kLatEvery),
				sphere.center.z + sphere.radius * std::cos(lat + kLatEvery) * std::sin(lon) };
			c = { sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon + kLonEvery),
				sphere.center.y + sphere.radius * std::sin(lat),
				sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon + kLonEvery) };

			Vector3 aNdc = Transform(a, worldViewProjectionMatrix);
			Vector3 aScreen = Transform(aNdc, viewportMatrix);
			Vector3 bNdc = Transform(b, worldViewProjectionMatrix);
			Vector3 bScreen = Transform(bNdc, viewportMatrix);
			Vector3 cNdc = Transform(c, worldViewProjectionMatrix);
			Vector3 cScreen = Transform(cNdc, viewportMatrix);
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(bScreen.x), int(bScreen.y), color);
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(cScreen.x), int(cScreen.y), color);
		}
	}
}

void DrawPlane(const Plane& plane, const Matrix4x4& worldViewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	Vector3 center = Vector3Multiply(plane.distance, plane.normal);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z };
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z };

	Vector3 points[4];
	for (int index = 0; index < 4; ++index)
	{
		Vector3 extend = Vector3Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, worldViewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), color);
}

Vector3 Perpendicular(const Vector3& vector)
{
	if (vector.x != 0.0f || vector.y != 0.0f)
	{
		return { -vector.y,vector.x,0.0f };
	}

	return { 0.0f,-vector.z,vector.y };
}

Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	Vector3 ret;
	ret = { (v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x) };
	return ret;
}

bool isCollision(const Ball& ball, const Plane& plane)
{
	float k = Dot(plane.normal, ball.position) - plane.distance;
	k = fabs(k);

	if (k <= ball.radius)
	{
		return true;
	}

	return false;
}

Vector3 Reflect(const Vector3& input, const Vector3& normal)
{
	Vector3 ret;
	ret.x = input.x - ( 2.0f * (input.x * normal.x) * normal.x);
	ret.y = input.y - ( 2.0f * (input.y * normal.y) * normal.y);
	ret.x = input.z - ( 2.0f * (input.z * normal.z) * normal.z);

	return ret;
	
}

Vector3 Bounce(Ball& ball, const Plane& plane,const float deltaTime)
{
	Vector3 ret;
	float e = 0.8f; // 反発係数

	ball.velocity = Add(ball.velocity, Vector3Multiply(deltaTime,ball.acceleration));
	ball.position = Add(ball.position, Vector3Multiply(deltaTime, ball.velocity));

	if (isCollision(ball, plane))
	{
		if(IsCollisionCapsule(ball, plane, deltaTime))
		{
			// カプセルと平面の衝突判定がtrueなら、ボールの位置を修正
			float penetration = ball.radius - (Dot(plane.normal, ball.position) - plane.distance);
			if (penetration > 0.0f)
			{
				ball.position = Add(ball.position, Vector3Multiply(penetration, plane.normal));
			}
		}

		Vector3 reflected = Reflect(ball.velocity, plane.normal);
		Vector3 projectToNormal = Project(ball.velocity, plane.normal);
		Vector3 movingDirection = Subtract(reflected, projectToNormal);
		ball.velocity = Add(Vector3Multiply(e, projectToNormal), movingDirection);

		// 吹っ飛びすぎた場合は制限
		const float maxSpeed = 100.0f;
		if (Length(ball.velocity) > maxSpeed)
		{
			ball.velocity = Vector3Multiply(maxSpeed / Length(ball.velocity), ball.velocity);
		}
	}

	ret = ball.position;
	return ret;
}

bool IsCollisionCapsule(const Ball& ball,const Plane& plane,const float deltaTime)
{
	// カプセル判定に使う変数の定義
	Capsule capsule;
	capsule.segment.origin = ball.position;
	capsule.segment.diff = Add(ball.position, Vector3Multiply(deltaTime, ball.velocity));

	// カプセル判定の計算
	Vector3 d = Vector3Multiply(-1.0f, plane.normal);
	Vector3 ba = Subtract(capsule.segment.diff, capsule.segment.origin);
	// baを正規化
	Vector3 e = Normalize(ba);

	if (fabsf(Dot(plane.normal, ba)) < 1e-6f)
	{
		if (fabsf(Dot(plane.normal, capsule.segment.origin) - plane.distance) <= ball.radius)
		{
			return true;
		}
		if (fabsf(Dot(plane.normal, capsule.segment.diff) - plane.distance) <= ball.radius)
		{
			return true;
		}
		return false;
	}
	// tの値を求める
	float t = (plane.distance - Dot(plane.normal, capsule.segment.origin)) / Dot(plane.normal, ba);
	// tを0~1の範囲に収める
	t = std::clamp(t, 0.0f, 1.0f);
	// 線形補間
	Vector3 f = Add(Vector3Multiply((1.0f - t), capsule.segment.origin), Vector3Multiply(t, capsule.segment.diff));
	// 距離を求める
	float distance = fabsf(Dot(plane.normal, f) - plane.distance);

	// 衝突判定
	if (distance <= ball.radius)
	{
		return true;
	}

	return false;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	Matrix4x4 ret;
	float cot = 1.0f / tanf(fovY / 2.0f);
	ret.m[0][0] = (1.0f / aspectRatio) * cot; ret.m[0][1] = 0.0f; ret.m[0][2] = 0.0f; ret.m[0][3] = 0.0f;
	ret.m[1][0] = 0.0f; ret.m[1][1] = cot; ret.m[1][2] = 0.0f; ret.m[1][3] = 0.0f;
	ret.m[2][0] = 0.0f; ret.m[2][1] = 0.0f; ret.m[2][2] = farClip / (farClip - nearClip); ret.m[2][3] = 1.0f;
	ret.m[3][0] = 0.0f; ret.m[3][1] = 0.0f; ret.m[3][2] = ((-1.0f * nearClip) * farClip) / (farClip - nearClip); ret.m[3][3] = 0.0f;

	return ret;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
{
	Matrix4x4 ret;
	ret.m[0][0] = 2.0f / (right - left); ret.m[0][1] = 0.0f; ret.m[0][2] = 0.0f; ret.m[0][3] = 0.0f;
	ret.m[1][0] = 0.0f; ret.m[1][1] = 2.0f / (top - bottom); ret.m[1][2] = 0.0f; ret.m[1][3] = 0.0f;
	ret.m[2][0] = 0.0f; ret.m[2][1] = 0.0f; ret.m[2][2] = 1.0f / (farClip - nearClip); ret.m[2][3] = 0.0f;
	ret.m[3][0] = (left + right) / (left - right); ret.m[3][1] = (top + bottom) / (bottom - top); ret.m[3][2] = nearClip / (nearClip - farClip); ret.m[3][3] = 1.0f;
	return ret;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth)
{
	Matrix4x4 ret;
	ret.m[0][0] = width / 2.0f; ret.m[0][1] = 0.0f; ret.m[0][2] = 0.0f; ret.m[0][3] = 0.0f;
	ret.m[1][0] = 0.0f; ret.m[1][1] = -1.0f * (height / 2); ret.m[1][2] = 0.0f; ret.m[1][3] = 0.0f;
	ret.m[2][0] = 0.0f; ret.m[2][1] = 0.0f; ret.m[2][2] = maxDepth - minDepth; ret.m[2][3] = 0.0f;
	ret.m[3][0] = left + (width / 2.0f); ret.m[3][1] = top + (height / 2.0f); ret.m[3][2] = minDepth; ret.m[3][3] = 1.0f;
	return ret;
}

Matrix4x4 MakeRotateXMatrix(float radian)
{
	Matrix4x4 ret;
	ret.m[0][0] = 1.0f; ret.m[0][1] = 0.0f; ret.m[0][2] = 0.0f; ret.m[0][3] = 0.0f;
	ret.m[1][0] = 0.0f; ret.m[1][1] = std::cos(radian); ret.m[1][2] = std::sin(radian); ret.m[1][3] = 0.0f;
	ret.m[2][0] = 0.0f; ret.m[2][1] = -std::sin(radian); ret.m[2][2] = std::cos(radian); ret.m[2][3] = 0.0f;
	ret.m[3][0] = 0.0f; ret.m[3][1] = 0.0f; ret.m[3][2] = 0.0f; ret.m[3][3] = 1.0f;
	return ret;
}

Matrix4x4 MakeRotateYMatrix(float radian)
{
	Matrix4x4 ret;
	ret.m[0][0] = std::cos(radian); ret.m[0][1] = 0.0f; ret.m[0][2] = -std::sin(radian); ret.m[0][3] = 0.0f;
	ret.m[1][0] = 0.0f; ret.m[1][1] = 1.0f; ret.m[1][2] = 0.0f; ret.m[1][3] = 0.0f;
	ret.m[2][0] = std::sin(radian); ret.m[2][1] = 0.0f; ret.m[2][2] = std::cos(radian); ret.m[2][3] = 0.0f;
	ret.m[3][0] = 0.0f; ret.m[3][1] = 0.0f; ret.m[3][2] = 0.0f; ret.m[3][3] = 1.0f;
	return ret;
}

Matrix4x4 MakeRotateZMatrix(float radian)
{
	Matrix4x4 ret;
	ret.m[0][0] = std::cos(radian); ret.m[0][1] = std::sin(radian); ret.m[0][2] = 0.0f; ret.m[0][3] = 0.0f;
	ret.m[1][0] = -std::sin(radian); ret.m[1][1] = std::cos(radian); ret.m[1][2] = 0.0f; ret.m[1][3] = 0.0f;
	ret.m[2][0] = 0.0f; ret.m[2][1] = 0.0f; ret.m[2][2] = 1.0f; ret.m[2][3] = 0.0f;
	ret.m[3][0] = 0.0f; ret.m[3][1] = 0.0f; ret.m[3][2] = 0.0f; ret.m[3][3] = 1.0f;
	return ret;
}

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2)
{
	Matrix4x4 ret;
	ret.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	ret.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	ret.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	ret.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];
	ret.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	ret.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	ret.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	ret.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];
	ret.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	ret.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	ret.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	ret.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];
	ret.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	ret.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	ret.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	ret.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return ret;
}


Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
{
	Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateY = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZ = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZ = Multiply(rotateX, Multiply(rotateY, rotateZ));

	Matrix4x4 ret;
	ret.m[0][0] = scale.x * rotateXYZ.m[0][0]; ret.m[0][1] = scale.x * rotateXYZ.m[0][1]; ret.m[0][2] = scale.x * rotateXYZ.m[0][2]; ret.m[0][3] = 0.0f;
	ret.m[1][0] = scale.y * rotateXYZ.m[1][0]; ret.m[1][1] = scale.y * rotateXYZ.m[1][1]; ret.m[1][2] = scale.y * rotateXYZ.m[1][2]; ret.m[1][3] = 0.0f;
	ret.m[2][0] = scale.z * rotateXYZ.m[2][0]; ret.m[2][1] = scale.z * rotateXYZ.m[2][1]; ret.m[2][2] = scale.z * rotateXYZ.m[2][2]; ret.m[2][3] = 0.0f;
	ret.m[3][0] = translate.x; ret.m[3][1] = translate.y; ret.m[3][2] = translate.z; ret.m[3][3] = 1.0f;

	return ret;

}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

Vector3 Add(const Vector3& v1, const Vector3& v2)
{
	Vector3 ret;
	ret.x = v1.x + v2.x;
	ret.y = v1.y + v2.y;
	ret.z = v1.z + v2.z;
	return ret;
}

Vector3 Divide(const Vector3& v, const float& scalar)
{
	Vector3 ret;
	if (scalar != 0.0f)
	{
		ret.x = v.x / scalar;
		ret.y = v.y / scalar;
		ret.z = v.z / scalar;
	}
	else
	{
		ret.x = 0.0f;
		ret.y = 0.0f;
		ret.z = 0.0f;
	}

	return ret;
}

Vector3 Subtract(const Vector3& a, const Vector3& b)
{
	Vector3 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

Vector3 Vector3Multiply(const float& v1, const Vector3& v2)
{
	Vector3 ret;
	ret.x = v1 * v2.x;
	ret.y = v1 * v2.y;
	ret.z = v1 * v2.z;
	return ret;
}

float Length(const Vector3& v)
{
	float ret;
	ret = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return ret;
}

Vector3 Normalize(const Vector3& v)
{
	Vector3 ret;
	float length = Length(v);

	if (length != 0)
	{
		ret.x = v.x / length;
		ret.y = v.y / length;
		ret.z = v.z / length;
	}
	else
	{
		ret.x = 0;
		ret.y = 0;
		ret.z = 0;
	}

	return ret;
}

Vector3 Project(const Vector3& v1, const Vector3& v2)
{
	Vector3 ret;
	float dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	float length = v2.x * v2.x + v2.y * v2.y + v2.z * v2.z;
	float scale = dot / length;
	ret.x = v2.x * scale;
	ret.y = v2.y * scale;
	ret.z = v2.z * scale;
	return ret;
}

float Dot(const Vector3& v1, const Vector3& v2)
{
	float ret;
	ret = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return ret;
}

Matrix4x4 Inverse(const Matrix4x4& m)
{
	Matrix4x4 ret;
	float a = (m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2])
		- (m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2])
		- (m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2])
		+ (m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2])
		+ (m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2])
		- (m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2])
		- (m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0])
		+ (m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]);

	ret.m[0][0] = (1.0f / a) * ((m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[1][3] * m.m[2][1] * m.m[3][2])
		- (m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[1][1] * m.m[2][3] * m.m[3][2]));

	ret.m[0][1] = (1.0f / a) * ((-1.0f * (m.m[0][1] * m.m[2][2] * m.m[3][3])) - (m.m[0][2] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[2][1] * m.m[3][2])
		+ (m.m[0][3] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[2][3] * m.m[3][2]));

	ret.m[0][2] = (1.0f / a) * ((m.m[0][1] * m.m[1][2] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[3][2])
		- (m.m[0][3] * m.m[1][2] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[3][2]));

	ret.m[0][3] = (1.0f / a) * ((-1.0f * (m.m[0][1] * m.m[1][2] * m.m[2][3])) - (m.m[0][2] * m.m[1][3] * m.m[2][1]) - (m.m[0][3] * m.m[1][1] * m.m[2][2])
		+ (m.m[0][3] * m.m[1][2] * m.m[2][1]) + (m.m[0][2] * m.m[1][1] * m.m[2][3]) + (m.m[0][1] * m.m[1][3] * m.m[2][2]));

	ret.m[1][0] = (1.0f / a) * (-1.0f * ((m.m[1][0] * m.m[2][2] * m.m[3][3])) - (m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[1][3] * m.m[2][0] * m.m[3][2])
		+ (m.m[1][3] * m.m[2][2] * m.m[3][0]) + (m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[1][0] * m.m[2][3] * m.m[3][2]));

	ret.m[1][1] = (1.0f / a) * ((m.m[0][0] * m.m[2][2] * m.m[3][3]) + (m.m[0][2] * m.m[2][3] * m.m[3][0]) + (m.m[0][3] * m.m[2][0] * m.m[3][2])
		- (m.m[0][3] * m.m[2][2] * m.m[3][0]) - (m.m[0][2] * m.m[2][1] * m.m[3][3]) - (m.m[0][0] * m.m[2][3] * m.m[3][2]));

	ret.m[1][2] = (1.0f / a) * (-1.0f * ((m.m[0][0] * m.m[1][2] * m.m[3][3])) - (m.m[0][2] * m.m[1][3] * m.m[3][0]) - (m.m[0][3] * m.m[1][0] * m.m[3][2])
		+ (m.m[0][3] * m.m[1][2] * m.m[3][0]) + (m.m[0][2] * m.m[1][0] * m.m[3][3]) + (m.m[0][1] * m.m[1][3] * m.m[3][2]));

	ret.m[1][3] = (1.0f / a) * ((m.m[0][0] * m.m[1][2] * m.m[2][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0]) + (m.m[0][3] * m.m[1][0] * m.m[2][2])
		- (m.m[0][3] * m.m[1][2] * m.m[2][0]) - (m.m[0][2] * m.m[1][0] * m.m[2][3]) - (m.m[0][0] * m.m[1][3] * m.m[2][2]));

	ret.m[2][0] = (1.0f / a) * ((m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[1][3] * m.m[2][0] * m.m[3][1])
		- (m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[1][0] * m.m[2][3] * m.m[3][1]));

	ret.m[2][1] = (1.0f / a) * ((-1.0f * (m.m[0][0] * m.m[2][1] * m.m[3][3])) - (m.m[0][1] * m.m[2][3] * m.m[3][0]) - (m.m[0][3] * m.m[2][0] * m.m[3][1])
		+ (m.m[0][3] * m.m[2][1] * m.m[3][0]) + (m.m[0][1] * m.m[2][0] * m.m[3][3]) + (m.m[0][0] * m.m[2][3] * m.m[3][1]));

	ret.m[2][2] = (1.0f / a) * ((m.m[0][0] * m.m[1][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][3] * m.m[3][0]) + (m.m[0][3] * m.m[1][0] * m.m[3][1])
		- (m.m[0][3] * m.m[1][1] * m.m[3][0]) - (m.m[0][1] * m.m[1][0] * m.m[3][3]) - (m.m[0][0] * m.m[1][3] * m.m[3][1]));

	ret.m[2][3] = (1.0f / a) * ((-1.0f * (m.m[0][0] * m.m[1][1] * m.m[2][3])) - (m.m[0][1] * m.m[1][3] * m.m[2][0]) - (m.m[0][3] * m.m[1][0] * m.m[2][1])
		+ (m.m[0][3] * m.m[1][1] * m.m[2][0]) + (m.m[0][1] * m.m[1][0] * m.m[2][3]) + (m.m[0][0] * m.m[1][3] * m.m[2][1]));

	ret.m[3][0] = (1.0f / a) * (-1.0f * ((m.m[1][0] * m.m[2][1] * m.m[3][2])) - (m.m[1][1] * m.m[2][2] * m.m[3][0]) - (m.m[1][2] * m.m[2][0] * m.m[3][1])
		+ (m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[1][1] * m.m[2][0] * m.m[3][2]) + (m.m[1][1] * m.m[2][2] * m.m[2][1]));

	ret.m[3][1] = (1.0f / a) * ((m.m[0][0] * m.m[2][1] * m.m[3][2]) + (m.m[0][1] * m.m[2][2] * m.m[3][0]) + (m.m[0][2] * m.m[2][0] * m.m[3][1])
		- (m.m[0][2] * m.m[2][1] * m.m[3][0]) - (m.m[0][1] * m.m[2][0] * m.m[3][2]) - (m.m[0][0] * m.m[2][2] * m.m[3][1]));

	ret.m[3][2] = (1.0f / a) * (-1.0f * ((m.m[0][0] * m.m[1][1] * m.m[3][2])) - (m.m[0][1] * m.m[1][2] * m.m[3][0]) - (m.m[0][2] * m.m[1][0] * m.m[3][1])
		+ (m.m[0][2] * m.m[1][1] * m.m[3][0]) + (m.m[0][1] * m.m[1][0] * m.m[3][2]) + (m.m[0][0] * m.m[1][2] * m.m[3][1]));

	ret.m[3][3] = (1.0f / a) * ((m.m[0][0] * m.m[1][1] * m.m[2][2]) + (m.m[0][1] * m.m[1][2] * m.m[2][0]) + (m.m[0][2] * m.m[1][0] * m.m[2][1])
		- (m.m[0][2] * m.m[1][1] * m.m[2][0]) - (m.m[0][1] * m.m[1][0] * m.m[2][2]) - (m.m[0][0] * m.m[1][2] * m.m[2][1]));

	return ret;

}
