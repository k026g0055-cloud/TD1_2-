#pragma once

#include "InputManager.h"
#include "GameTimer.h"

// QTEの操作方式
enum QTEMode
{
    QTE_BEGINNER,
    QTE_ADVANCED
};

// QTEの状態
enum QTEState
{
    QTE_IDLE,
    QTE_PLAYING,
    QTE_SUCCESS,
    QTE_FAILED
};

// QTEで使用するコマンド
enum QTECommand
{
    CMD_NONE,

    // ボタン
    CMD_A,
    CMD_B,
    CMD_X,
    CMD_Y,

    // スティック8方向
    CMD_UP,
    CMD_DOWN,
    CMD_LEFT,
    CMD_RIGHT,

    CMD_UP_LEFT,
    CMD_UP_RIGHT,
    CMD_DOWN_LEFT,
    CMD_DOWN_RIGHT
};

class QTESystem
{
public:
    void Initialize();

    // QTE開始
    void Start(QTEMode mode);

    // QTE更新
    void Update(const InputManager& input);

    // QTE描画
    void Draw() const;

    void Debug();
    void DrawXboxButtons() const;

    // 初期状態に戻す
    void Reset();

    // 状態取得
    bool IsPlaying() const;
    bool IsSuccess() const;
    bool IsFailed() const;

    QTEMode GetMode() const;

    // ダメージ取得
    int GetDamage() const;

private:
    static const int kMaxSets = 3;
    static const int kMaxCommands = 4;

    // 同じ方向を維持するフレーム数
    int stickHoldTimer_;

    // 入力確定に必要なフレーム数
    static const int kStickHoldFrames = 2;

    // 現在維持している方向
    QTECommand holdingDirection_;

    // 同じ方向を連続確定させないための記録
    QTECommand confirmedDirection_;

    QTEMode mode_;
    QTEState state_;

    // コマンド配列
    QTECommand commands_[kMaxSets][kMaxCommands];
    int commandCounts_[kMaxSets];

    // 現在の進行
    int currentSet_;
    int currentCommand_;

    // 制限時間
    GameTimer timer_;
    int timeLimit_;

    // ダメージ
    int damage_;

    //コマンド猶予フレーム//
    int commandTimer_;
    //初心者
    const int kCommandBeginnerTimeLimit = 30;
    //上級者
    const int kCommandAdvancedTimeLimit = 12;

    // Xboxボタンの表示設定
    const float buttonCenterX_ = 1100.0f;
    const float buttonCenterY_ = 570.0f;
    const float buttonDistance_ = 60.0f;
    const int buttonRadius_ = 25;

    // 前フレームの方向
    QTECommand previousDirection_;

    // コマンド設定
    void SetupCommands();

    // 次のセットへ
    void NextSet();

    // 矢印を描画
    void DrawCommandArrow(
        QTECommand command,
        int centerX,
        int centerY,
        unsigned int color
    ) const;

    // ボタン入力取得
    QTECommand GetButtonInput(
        const InputManager& input) const;

    // スティック入力取得
    QTECommand GetStickInput(
        const InputManager& input);

    // コマンド名取得
    const char* GetCommandText(
        QTECommand command) const;
};
