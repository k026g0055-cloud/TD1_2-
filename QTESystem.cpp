
#include "QTESystem.h"

#include <Novice.h>
#include <Xinput.h>

// 初期化
void QTESystem::Initialize()
{
    mode_ = QTE_BEGINNER;
    state_ = QTE_IDLE;

    currentSet_ = 0;
    currentCommand_ = 0;

    timer_.Reset();
    timeLimit_=180;

    damage_ = 0;

    stickHoldTimer_ = 0;

    holdingDirection_ = CMD_NONE;
    confirmedDirection_ = CMD_NONE;
    previousDirection_ = CMD_NONE;

    for (int i = 0; i < kMaxSets; i++)
    {
        commandCounts_[i] = 0;

        for (int j = 0; j < kMaxCommands; j++)
        {
            commands_[i][j] = CMD_NONE;
        }
    }
}

// QTE開始
void QTESystem::Start(QTEMode mode)
{
    Initialize();

    mode_ = mode;
    state_ = QTE_PLAYING;

    SetupCommands();
}

// コマンド設定
void QTESystem::SetupCommands()
{
    if (mode_ == QTE_BEGINNER)
    {
        // 1セット目
        commands_[0][0] = CMD_A;
        commands_[0][1] = CMD_B;
        commandCounts_[0] = 2;

        // 2セット目
        commands_[1][0] = CMD_X;
        commands_[1][1] = CMD_A;
        commands_[1][2] = CMD_Y;
        commandCounts_[1] = 3;

        // 3セット目
        commands_[2][0] = CMD_B;
        commands_[2][1] = CMD_X;
        commands_[2][2] = CMD_Y;
        commands_[2][3] = CMD_A;
        commandCounts_[2] = 4;
    }
    else
    {
        // 1セット目：→ ↓ ↘
        commands_[0][0] = CMD_RIGHT;
        commands_[0][1] = CMD_DOWN;
        commands_[0][2] = CMD_DOWN_RIGHT;
        commandCounts_[0] = 3;

        // 2セット目：↓ ↘ →
        commands_[1][0] = CMD_DOWN;
        commands_[1][1] = CMD_DOWN_RIGHT;
        commands_[1][2] = CMD_RIGHT;
        commandCounts_[1] = 3;

        // 3セット目：→ ↓ ↘ →
        commands_[2][0] = CMD_RIGHT;
        commands_[2][1] = CMD_DOWN;
        commands_[2][2] = CMD_DOWN_RIGHT;
        commands_[2][3] = CMD_RIGHT;
        commandCounts_[2] = 4;
    }

    timeLimit_ = 180;
    timer_.Start(timeLimit_);
}

// ボタン入力
QTECommand QTESystem::GetButtonInput(
    const InputManager& input) const
{
    if (input.IsButtonTrigger(XINPUT_GAMEPAD_A) ||
        input.IsKeyTrigger(DIK_J))
    {
        return CMD_A;
    }

    if (input.IsButtonTrigger(XINPUT_GAMEPAD_B) ||
        input.IsKeyTrigger(DIK_K))
    {
        return CMD_B;
    }

    if (input.IsButtonTrigger(XINPUT_GAMEPAD_X) ||
        input.IsKeyTrigger(DIK_U))
    {
        return CMD_X;
    }

    if (input.IsButtonTrigger(XINPUT_GAMEPAD_Y) ||
        input.IsKeyTrigger(DIK_I))
    {
        return CMD_Y;
    }

    return CMD_NONE;
}

// スティック入力


QTECommand QTESystem::GetStickInput(
    const InputManager& input)
{
    Vector2 stick = input.GetMoveDirection();

    QTECommand direction = CMD_NONE;

    float lengthSquared =
        stick.x * stick.x + stick.y * stick.y;

    if (lengthSquared >= 0.25f)
    {
        float x = stick.x;
        float y = stick.y;

        // 斜め方向
        if (x > 0.0f && y > 0.0f &&
            x > y * 0.5f && y > x * 0.5f)
        {
            direction = CMD_DOWN_RIGHT;
        }
        else if (x < 0.0f && y > 0.0f &&
            -x > y * 0.5f && y > -x * 0.5f)
        {
            direction = CMD_DOWN_LEFT;
        }
        else if (x > 0.0f && y < 0.0f &&
            x > -y * 0.5f && -y > x * 0.5f)
        {
            direction = CMD_UP_RIGHT;
        }
        else if (x < 0.0f && y < 0.0f &&
            -x > -y * 0.5f && -y > -x * 0.5f)
        {
            direction = CMD_UP_LEFT;
        }
        else if (x > 0.0f && x >= y && x >= -y)
        {
            direction = CMD_RIGHT;
        }
        else if (x < 0.0f && -x >= y && -x >= -y)
        {
            direction = CMD_LEFT;
        }
        else if (y > 0.0f)
        {
            direction = CMD_DOWN;
        }
        else
        {
            direction = CMD_UP;
        }
    }

    // ニュートラルならリセット
    if (direction == CMD_NONE)
    {
        holdingDirection_ = CMD_NONE;
        confirmedDirection_ = CMD_NONE;
        stickHoldTimer_ = 0;
        return CMD_NONE;
    }

    // 方向が変わったらカウントし直す
    if (direction != holdingDirection_)
    {
        holdingDirection_ = direction;
        stickHoldTimer_ = 0;
    }

    // 同じ方向を維持している間カウント
    stickHoldTimer_++;

    // 一定フレーム維持したら入力確定
    if (stickHoldTimer_ >= kStickHoldFrames)
    {
        // すでに確定済みの方向は繰り返さない
        if (direction != confirmedDirection_)
        {
            confirmedDirection_ = direction;
            return direction;
        }
    }

    return CMD_NONE;
}

// QTE更新
void QTESystem::Update(const InputManager& input)
{
    if (state_ != QTE_PLAYING)
    {
        return;
    }

    timer_.Update();

    // 時間切れ
    if (timer_.IsFinishes())
    {
        state_ = QTE_FAILED;
        return;
    }

    QTECommand command = CMD_NONE;

    if (mode_ == QTE_BEGINNER)
    {
        command = GetButtonInput(input);
    }
    else
    {
        command = GetStickInput(input);
    }

    if (command == CMD_NONE)
    {
        return;
    }

    QTECommand correct =
        commands_[currentSet_][currentCommand_];

    if (command == correct)
    {
        currentCommand_++;

        if (currentCommand_ >= commandCounts_[currentSet_])
        {
            NextSet();
        }
    }
    else
    {
        state_ = QTE_FAILED;
    }
}

// 次のセット

void QTESystem::NextSet()
{
    damage_ += 100;

    currentSet_++;
    currentCommand_ = 0;

    if (currentSet_ >= kMaxSets)
    {
        damage_ += 200;
        state_ = QTE_SUCCESS;
        return;
    }

    timer_.Start(timeLimit_);

    stickHoldTimer_ = 0;
    holdingDirection_ = CMD_NONE;

    // confirmedDirection_はリセットしない
    // 同じ方向を倒しっぱなしで次のセットに進まないようにする
}

// コマンド名
const char* QTESystem::GetCommandText(
    QTECommand command) const
{
    switch (command)
    {
    case CMD_A: return "A";
    case CMD_B: return "B";
    case CMD_X: return "X";
    case CMD_Y: return "Y";

    case CMD_UP: return "UP";
    case CMD_DOWN: return "DOWN";
    case CMD_LEFT: return "LEFT";
    case CMD_RIGHT: return "RIGHT";

    case CMD_UP_LEFT: return "UP-LEFT";
    case CMD_UP_RIGHT: return "UP-RIGHT";
    case CMD_DOWN_LEFT: return "DOWN-LEFT";
    case CMD_DOWN_RIGHT: return "DOWN-RIGHT";

    default: return "";
    }
}


void QTESystem::DrawCommandArrow(
    QTECommand command,
    int centerX,
    int centerY,
    unsigned int color
) const
{
    int dx = 0;
    int dy = 0;

    switch (command)
    {
    case CMD_UP:
        dy = -1;
        break;

    case CMD_DOWN:
        dy = 1;
        break;

    case CMD_LEFT:
        dx = -1;
        break;

    case CMD_RIGHT:
        dx = 1;
        break;

    case CMD_UP_LEFT:
        dx = -1;
        dy = -1;
        break;

    case CMD_UP_RIGHT:
        dx = 1;
        dy = -1;
        break;

    case CMD_DOWN_LEFT:
        dx = -1;
        dy = 1;
        break;

    case CMD_DOWN_RIGHT:
        dx = 1;
        dy = 1;
        break;

    default:
        return;
    }

    // 矢印の長さ
    int length = 22;

    // 矢印の先端
    int endX = centerX + dx * length;
    int endY = centerY + dy * length;

    // 矢印の軸
    Novice::DrawLine(
        centerX, centerY,
        endX, endY,
        color
    );

    // 矢印の羽根
    int wing = 9;

    int backX = endX - dx * wing;
    int backY = endY - dy * wing;

    int sideX = -dy * wing;
    int sideY = dx * wing;

    Novice::DrawLine(
        endX, endY,
        backX + sideX,
        backY + sideY,
        color
    );

    Novice::DrawLine(
        endX, endY,
        backX - sideX,
        backY - sideY,
        color
    );
}


// 描画
void QTESystem::Draw() const
{
    if (state_ == QTE_IDLE)
    {
        return;
    }

    // QTE背景
    Novice::DrawBox(
        290, 150, 700, 350,
        0.0f, 0x202030FF,
        kFillModeSolid
    );

    if (state_ == QTE_SUCCESS)
    {
        Novice::ScreenPrintf(
            530, 280,
            "QTE SUCCESS!"
        );

        Novice::ScreenPrintf(
            530, 310,
            "DAMAGE : %d", damage_
        );

        return;
    }

    if (state_ == QTE_FAILED)
    {
        Novice::ScreenPrintf(
            530, 280,
            "QTE FAILED"
        );

        Novice::ScreenPrintf(
            530, 310,
            "DAMAGE : %d", damage_
        );

        return;
    }

    Novice::ScreenPrintf(
        520, 180,
        "BREAK COMBO"
    );

    Novice::ScreenPrintf(
        500, 220,
        "SET : %d / 3",
        currentSet_ + 1
    );

    // コマンド表示
    for (int i = 0;
        i < commandCounts_[currentSet_];
        i++)
    {
        int x = 390 + i * 130;
        int y = 280;

        unsigned int color = 0x555555FF;

        if (i < currentCommand_)
        {
            color = 0x33AA66FF;
        }
        else if (i == currentCommand_)
        {
            color = 0x4488FFFF;
        }

        Novice::DrawBox(
            x, y, 110, 60,
            0.0f, color,
            kFillModeSolid
        );

        QTECommand command = commands_[currentSet_][i];

        if (mode_ == QTE_ADVANCED)
        {
            // 上級者は矢印で表示
            DrawCommandArrow(
                command,
                x + 55,
                y + 30,
                WHITE
            );
        }
        else
        {
            // 初心者はXYABを表示
            Novice::ScreenPrintf(
                x + 50,
                y + 20,
                "%s",
                GetCommandText(command)
            );
        }
    }

    // 制限時間ゲージ
    float rate =
        static_cast<float>(timer_.GetRemaining()) /
        static_cast<float>(timeLimit_);

    Novice::DrawBox(
        390, 400, 500, 20,
        0.0f, 0x444444FF,
        kFillModeSolid
    );

    Novice::DrawBox(
        390, 400,
        static_cast<int>(500 * rate),
        20,
        0.0f, 0xFFCC44FF,
        kFillModeSolid
    );

    Novice::ScreenPrintf(
        500, 440,
        "DAMAGE : %d",
        damage_
    );
}

// リセット
void QTESystem::Reset()
{
    Initialize();
}

// QTE実行中か
bool QTESystem::IsPlaying() const
{
    return state_ == QTE_PLAYING;
}

// QTE成功したか
bool QTESystem::IsSuccess() const
{
    return state_ == QTE_SUCCESS;
}

// QTE失敗したか
bool QTESystem::IsFailed() const
{
    return state_ == QTE_FAILED;
}

// 操作方式取得
QTEMode QTESystem::GetMode() const
{
    return mode_;
}

// ダメージ取得
int QTESystem::GetDamage() const
{
    return damage_;
}
