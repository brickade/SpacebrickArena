#include "include/GameScene.h"

#include "TheBrick/Conversion.h"

namespace Game
{
    CGameScene::CGameScene(PuRe_Application* a_pApplication, int a_playerIdx)
    {
        this->m_pApplication = a_pApplication;
        this->m_playerIdx = a_playerIdx;
    }

    void CGameScene::StartGame()
    {
        for (int i = 0; i < this->m_Players.size(); i++)
        {
            TheBrick::CSerializer serializer;
            serializer.OpenRead("../data/ships/banana.ship");
            ong::vec3 pos = ong::vec3(10.0f,10.0f,10.0f);
            pos.x += this->m_Players[i]->ID*10.0f;
            this->m_Players[i]->Ship = new TheBrick::CSpaceship(*BrickBozz::Instance()->World,pos);
            this->m_Players[i]->Ship->Deserialize(serializer, *BrickBozz::Instance()->BrickManager, *BrickBozz::Instance()->World);
            serializer.Close();
        }
        ong::vec3 start(50.0f, 50.0f, 50.0f);
        for (int i = 0; i < 10; i++)
        {
            TheBrick::CAsteroid* asteroid = new TheBrick::CAsteroid(BrickBozz::Instance()->BrickManager, *BrickBozz::Instance()->World, start + ong::vec3((i % 2)*10.0f, (i % 2) * 10.0f, i*5.0f));
            this->m_Asteroids.push_back(asteroid);
        }
        this->gameStart = true;

    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::ReceiveData()
    {
        char buffer[256];
        SOCKADDR_IN sender;
        while (true)
        {
            printf("Receiving Data!\n");
            if (this->m_pNetwork->Receive(buffer, 256, &sender) != -1)
            {
                ReceivePacket* Packet = (ReceivePacket*)buffer;
                if (Packet->Head.Type == 0)
                {
                    int ID = 0; // 0 is Host
                    for (int i = 0; i < this->m_Players.size(); i++)
                    {
                        if (ID == this->m_Players[i]->ID)
                        {
                            ID++;
                            i = 0;
                        }
                    }
                    Player* p = new Player();
                    p->ID = ID;
                    p->NetworkInformation = sender;
                    this->m_Players.push_back(p);
                    printf("User %i joined!\n", ID);
                    //Tell him who he is
                    LeftPacket lPacket;
                    lPacket.Head.Type = 2;
                    lPacket.Who = ID;
                    this->m_pNetwork->Send((char*)&lPacket, sizeof(LeftPacket), sender);

                    //Send to JOINER all existing players
                    lPacket.Head.Type = 3;
                    for (int i = 0; i < this->m_Players.size(); i++)
                    {
                        lPacket.Who = this->m_Players[i]->ID;
                        this->m_pNetwork->Send((char*)&lPacket, sizeof(LeftPacket), sender);
                        //Same call send this player about the JOINER
                        if (this->m_Players[i]->ID != ID)
                        {
                            lPacket.Who = ID;
                            this->m_pNetwork->Send((char*)&lPacket, sizeof(LeftPacket), this->m_Players[i]->NetworkInformation);
                        }
                    }
                }
                else if (Packet->Head.Type == 1)
                {
                    LeftPacket* lPacket = (LeftPacket*)Packet;

                    for (int i = 0; i < this->m_Players.size(); i++)
                    {
                        if (this->m_Players[i]->ID == lPacket->Who)
                        {
                            SAFE_DELETE(this->m_Players[i]);
                            this->m_Players.erase(this->m_Players.begin() + i);
                        }
                    }
                    printf("User % i left!\n", lPacket->Who);
                    //Send to everyone else that one left
                    if (this->m_pNetwork->m_Host)
                    {
                        for (int i = 0; i < this->m_Players.size(); i++)
                        {
                            this->m_pNetwork->Send((char*)lPacket, sizeof(LeftPacket), this->m_Players[i]->NetworkInformation);
                        }
                    }
                }
                else if (Packet->Head.Type == 2)
                {
                    LeftPacket* LPacket = (LeftPacket*)Packet;
                    this->m_ID = LPacket->Who;
                    printf("I am %i!\n", this->m_ID);
                }
                else if (Packet->Head.Type == 3)
                {
                    LeftPacket* LPacket = (LeftPacket*)Packet;
                    Player* p = new Player();
                    p->ID = LPacket->Who;
                    if (p->ID == this->m_ID)
                        this->m_ArrayID = this->m_Players.size();
                    p->NetworkInformation = sender;
                    this->m_Players.push_back(p);
                    printf("User %i joined!\n", p->ID);

                }
                else if (Packet->Head.Type == 4)
                {
                    this->StartGame();
                }
                else if (Packet->Head.Type == 5)
                {
                    InputBasePacket* IPacket = (InputBasePacket*)Packet;
                    for (int i = 0; i < this->m_Players.size(); i++)
                    {
                        if (this->m_Players[i]->ID == IPacket->Who)
                        {
                            if (IPacket->Input == 0)
                            {
                                this->m_Players[i]->Ship->Shoot(this->m_Bullets, BrickBozz::Instance()->BrickManager);
                            }
                            else if (IPacket->Input == 1)
                            {
                                MovePacket* MPacket = (MovePacket*)Packet;
                                this->m_Players[i]->Ship->Move(MPacket->Move);
                            }
                            else if (IPacket->Input == 2)
                            {
                                ThrustPacket* TPacket = (ThrustPacket*)Packet;
                                this->m_Players[i]->Ship->Thrust(TPacket->Thrust);
                            }
                            else if (IPacket->Input == 3)
                            {
                                ThrustPacket* TPacket = (ThrustPacket*)Packet;
                                this->m_Players[i]->Ship->Spin(TPacket->Thrust);
                            }
                        }
                        //send to everyone that someone shot if HOST
                        if (this->m_pNetwork->m_Host&&i != 0)
                        {
                            if (IPacket->Input == 0)
                            {
                                this->m_pNetwork->Send((char*)IPacket, sizeof(InputBasePacket), this->m_Players[i]->NetworkInformation);
                            }
                            else if (IPacket->Input == 1)
                            {
                                MovePacket* MPacket = (MovePacket*)Packet;
                                this->m_pNetwork->Send((char*)MPacket, sizeof(MovePacket), this->m_Players[i]->NetworkInformation);
                            }
                            else if (IPacket->Input == 2||IPacket->Input == 3)
                            {
                                ThrustPacket* TPacket = (ThrustPacket*)Packet;
                                this->m_pNetwork->Send((char*)TPacket, sizeof(ThrustPacket), this->m_Players[i]->NetworkInformation);
                            }
                        }

                    }
                }
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        //Camera
        PuRe_Vector2F size = PuRe_Vector2F((float)gdesc.ResolutionWidth/2, (float)gdesc.ResolutionHeight);
        //this->m_pMaterial = a_pGraphics->LoadMaterial("../data/effects/GameEffects/default/default"); //Kann weg
        this->m_pFontMaterial = a_pGraphics->LoadMaterial("../data/effects/font/default");
        this->m_pUIMaterial = a_pGraphics->LoadMaterial("../data/effects/UI/default");
        this->m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/Post/default");
        this->m_pSkyMaterial = a_pGraphics->LoadMaterial("../data/effects/skybox/default");
        this->m_pPointLightMaterial = a_pGraphics->LoadMaterial("../data/effects/PointLight/default");
        this->m_pDirectionalLightMaterial = a_pGraphics->LoadMaterial("../data/effects/DirectionalLight/default");
        this->m_pModel = new PuRe_Model(a_pGraphics, "../data/models/brick1.obj");
        this->m_pSkyBox = new PuRe_SkyBox(a_pGraphics, "../data/textures/cube/");
        this->m_pPointLight = new PuRe_PointLight(a_pGraphics);
        this->m_pDirectionalLight = new PuRe_DirectionalLight(a_pGraphics);
        this->m_pMinimap = new CMinimap(a_pGraphics);
        this->m_pFont = new PuRe_Font(a_pGraphics, "../data/textures/font.png");
        this->m_pNetwork = new CNetworkHandler();

        this->m_MapBoundaries = PuRe_BoundingBox(PuRe_Vector3F(-1000.0f, -1000.0f, -1000.0f), PuRe_Vector3F(1000.0f, 1000.0f, 1000.0f));

        this->gameStart = false;
        this->m_pNetwork->m_NetworkState = 3;
        this->m_ID = 0;
        int playerNum = 2;
        for (int i=0;i<playerNum;i++)
        {
            CGameCamera* Cam = new CGameCamera(size, PuRe_Camera_Perspective);
            Cam->Initialize();
            this->m_Cameras.push_back(Cam);
            Player* p = new Player();
            p->ID = i;
            p->NetworkInformation = SOCKADDR_IN();
            this->m_Players.push_back(p);
        }

        //this->m_pPlayerShip = new TheBrick::CSpaceship();
        //this->m_pPlayerShip->Deserialize(nullptr, BrickBozz::Instance()->BrickManager, BrickBozz::Instance()->World);
        //ong::vec3 start(50.0f, 50.0f, 50.0f);
        //for (int i = 0; i < 10; i++)
        //{
        //    TheBrick::CAsteroid* asteroid = new TheBrick::CAsteroid(BrickBozz::Instance()->World, start + ong::vec3((i % 2)*5.0f,(i % 2) * 5.0f,i*2.0f));
        //    asteroid->Deserialize(nullptr, BrickBozz::Instance()->BrickManager, BrickBozz::Instance()->World);
        //    this->m_Asteroids.push_back(asteroid);
        //}
        this->StartGame();
        this->textureID = 0;
        this->physicsTimer = 0.0f;

    }

    // **************************************************************************
    // **************************************************************************
    bool CGameScene::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        //Handle ESC Button
        if (a_pInput->KeyPressed(a_pInput->ESC)
            || a_pInput->GamepadPressed(a_pInput->Pad_Back, this->m_playerIdx)
            || a_pInput->KeyPressed(a_pInput->F1)
            || a_pInput->GamepadPressed(a_pInput->Pad_Start, this->m_playerIdx))
        {
            return true;
        }

        BrickBozz::Instance()->UpdatePhysics(a_pTimer);

        //this->physicsTimer += a_pTimer->GetElapsedSeconds();
        //float constval = 1.0f / 60.0f;
        //while (this->physicsTimer > constval)
        //{
        //    BrickBozz::Instance()->World->step(constval);
        //    this->physicsTimer -= constval;
        //}


        if (a_pInput->KeyPressed(a_pInput->Left))
        {
            this->textureID--;
            if (this->textureID < 0)
                this->textureID = 4;
        }

        else if (a_pInput->KeyPressed(a_pInput->Right))
        {
            this->textureID++;
            if (this->textureID > 4)
                this->textureID = 0;
        }
        if (this->gameStart)
        {

            /*if (a_pInput->GamepadPressed(a_pInput->Pad_A,0))
            {
                InputBasePacket IPacket;
                IPacket.Head.Type = 5;
                IPacket.Input = 0;
                IPacket.Who = this->m_ID;
                if (this->m_pNetwork->m_Host)
                {
                    this->m_Players[this->m_ArrayID]->Ship->Shoot(this->m_Bullets, BrickBozz::Instance()->BrickManager);
                    for (int i = 1; i < this->m_Players.size(); i++)
                        this->m_pNetwork->Send((char*)&IPacket, sizeof(InputBasePacket), this->m_Players[i]->NetworkInformation);
                }
                else
                {
                    this->m_pNetwork->SendHost((char*)&IPacket, sizeof(InputBasePacket));
                }
            }

            PuRe_Vector2F Move = a_pInput->GetGamepadLeftThumb(0);
            if (Move.Length() > 0.5f)
            {
                MovePacket MPacket;
                MPacket.InputBase.Head.Type = 5;
                MPacket.InputBase.Input = 1;
                MPacket.InputBase.Who = this->m_ID;
                MPacket.Move = Move;
                if (this->m_pNetwork->m_Host)
                {
                    this->m_Players[this->m_ArrayID]->Ship->Move(Move);
                    for (int i = 1; i < this->m_Players.size(); i++)
                        this->m_pNetwork->Send((char*)&MPacket, sizeof(MovePacket), this->m_Players[i]->NetworkInformation);
                }
                else
                    this->m_pNetwork->SendHost((char*)&MPacket, sizeof(MovePacket));
            }

            float Thrust = a_pInput->GetGamepadRightTrigger(0);
            if (Thrust > 0.2f)
            {
                ThrustPacket TPacket;
                TPacket.InputBase.Head.Type = 5;
                TPacket.InputBase.Input = 2;
                TPacket.InputBase.Who = this->m_ID;
                TPacket.Thrust = Thrust;
                if (this->m_pNetwork->m_Host)
                {
                    this->m_Players[this->m_ArrayID]->Ship->Thrust(Thrust);
                    for (int i = 1; i < this->m_Players.size(); i++)
                        this->m_pNetwork->Send((char*)&TPacket, sizeof(ThrustPacket), this->m_Players[i]->NetworkInformation);
                }
                else
                    this->m_pNetwork->SendHost((char*)&TPacket, sizeof(ThrustPacket));
            }
            float Spin = 0.0f;
            if (a_pInput->GamepadIsPressed(a_pInput->Left_Shoulder, 0))
                Spin -= 1.0f;
            else if (a_pInput->GamepadIsPressed(a_pInput->Right_Shoulder, 0))
                Spin += 1.0f;
            if (Spin > 0.2f||Spin < -0.2f)
            {
                ThrustPacket TPacket;
                TPacket.InputBase.Head.Type = 5;
                TPacket.InputBase.Input = 3;
                TPacket.InputBase.Who = this->m_ID;
                TPacket.Thrust = Spin;
                if (this->m_pNetwork->m_Host)
                {
                    this->m_Players[this->m_ArrayID]->Ship->Spin(Spin);
                    for (int i = 1; i < this->m_Players.size(); i++)
                        this->m_pNetwork->Send((char*)&TPacket, sizeof(ThrustPacket), this->m_Players[i]->NetworkInformation);
                }
                else
                    this->m_pNetwork->SendHost((char*)&TPacket, sizeof(ThrustPacket));
            }*/

            //player->HandleInput(a_pInput, a_pTimer->GetElapsedSeconds(), this->m_Bullets, BrickBozz::Instance()->BrickManager);

            for (int i = 0; i<this->m_Players.size();i++)
            {
                TheBrick::CSpaceship* player = this->m_Players[i]->Ship;
                player->HandleInput(i, a_pInput, a_pTimer->GetElapsedSeconds(), this->m_Bullets, BrickBozz::Instance()->BrickManager);
                PuRe_Vector3F playerpos = TheBrick::OngToPuRe(player->m_pBody->getTransform().p);

                //Move player inside of map if hes outside
                float mapEnd = this->m_MapBoundaries.m_Position.X + this->m_MapBoundaries.m_Size.X;
                if (playerpos.X > mapEnd)
                    playerpos.X = playerpos.X - mapEnd;
                else if (playerpos.X < this->m_MapBoundaries.m_Position.X)
                    playerpos.X = mapEnd + playerpos.X;

                mapEnd = this->m_MapBoundaries.m_Position.Y + this->m_MapBoundaries.m_Size.Y;
                if (playerpos.Y > mapEnd)
                    playerpos.Y = playerpos.Y - mapEnd;
                else if (playerpos.Y < this->m_MapBoundaries.m_Position.Y)
                    playerpos.Y = mapEnd + playerpos.Y;

                mapEnd = this->m_MapBoundaries.m_Position.Z + this->m_MapBoundaries.m_Size.Z;
                if (playerpos.Z > mapEnd)
                    playerpos.Z = playerpos.Z - mapEnd;
                else if (playerpos.Z < this->m_MapBoundaries.m_Position.Z)
                    playerpos.Z = mapEnd + playerpos.Z;

                player->m_pBody->setPosition(TheBrick::PuReToOng(playerpos));
                player->Update(a_pTimer->GetElapsedSeconds());

                this->m_Cameras[i]->Update(i,this->m_Players[i]->Ship, a_pInput, a_pTimer);
            }
        }
        else
        {

            //Check last network state
            int networkState = this->m_pNetwork->GetState();

            this->m_pNetwork->Update(a_pInput); //Update Network State

            //If he connected
            if (networkState != 3 && this->m_pNetwork->GetState() == 3)
            {
                printf("Connecting!\n");
                this->m_pNetwork->Connect();
                //Add self as 0 if host
                if (this->m_pNetwork->m_Host)
                {
                    printf("You are the Host!\n");
                    Player* p = new Player();
                    p->ID = 0;
                    p->NetworkInformation = SOCKADDR_IN();
                    this->m_ArrayID = this->m_Players.size();
                    this->m_Players.push_back(p);
                    this->m_ID = p->ID;
                }
                //Start thread to listen
                std::thread receiveThread(&CGameScene::ReceiveData, this);
                receiveThread.detach();
            }

            if (a_pInput->KeyPressed(a_pInput->F3) && networkState == 3 && this->m_pNetwork->m_Host)
            {
                //Send to everyone that
                HeadPacket Packet;
                Packet.Type = 4;
                for (int i = 1; i < this->m_Players.size(); i++)
                {
                    this->m_pNetwork->Send((char*)&Packet, sizeof(HeadPacket), this->m_Players[i]->NetworkInformation);
                }
                this->StartGame();
            }
        }

        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
        {
            this->m_Bullets[i]->Update(a_pTimer->GetElapsedSeconds());

            if (this->m_Bullets[i]->m_lifeTime > 5.0f)
            {
                SAFE_DELETE(this->m_Bullets[i]);
                if (this->m_Bullets.begin() + i < this->m_Bullets.end())
                    this->m_Bullets.erase(this->m_Bullets.begin() + i);
                i--;
            }
        }


        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Render(PuRe_IGraphics* a_pGraphics)
    {
        PuRe_Color clear = PuRe_Color(0.0f, 0.4f, 1.0f);
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        PuRe_Renderer* renderer = BrickBozz::Instance()->Renderer;
        renderer->Begin(PuRe_Color(0.1f, 0.5f, 0.1f));


        /////////////  DRAW Light  ///////////////////////
        renderer->Draw(this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 0.0f), PuRe_Color(0.3f, 0.3f, 0.3f));
        /////////////  DRAW SKY  ///////////////////////
        renderer->Draw(this->m_pSkyBox, this->m_pSkyMaterial);
        ////////////////////////////////////////////////////

        /////////////  DRAW BRICKS  ///////////////////////
        BrickBozz::Instance()->BrickManager->Render(*BrickBozz::Instance()->Renderer);
        ////////////////////////////////////////////////////

        /////////////  DRAW MINIMAP  ////////////////////////
        //PuRe_Vector3F minipos = PuRe_Vector3F(gdesc.ResolutionWidth - 150.0f, gdesc.ResolutionHeight - 150.0f, 128.0f);
        //PuRe_Vector3F rot = this->m_pCamera->GetRotation();
        //rot *= PuRe_DegToRad;
        //PuRe_MatrixF rotation = PuRe_QuaternionF(rot).GetMatrix();
        //this->m_pMinimap->Draw(renderer, this->m_pUIMaterial, minipos, rotation);
        // Draw Players
        if (this->gameStart)
        {
 /*           for (int i = 0; i < this->m_Players.size(); i++)
            {
                this->m_pMinimap->DrawPlayer(renderer, this->m_pUIMaterial, TheBrick::OngToPuRe(this->m_Players[i]->Ship->m_pBody->getWorldCenter()), this->m_MapBoundaries, rotation);
            }*/
            //for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
            //    this->m_Bullets[i]->Draw(a_pGraphics, this->m_pCamera);
            //for (unsigned int i = 0; i < this->m_Asteroids.size(); i++)
            //    this->m_Asteroids[i]->Draw(a_pGraphics, this->m_pCamera);
        }
        ////////////////////////////////////////////////////

        //////////////////////  NETWORK UI  /////////////////////////////
        int nstate = this->m_pNetwork->GetState();
        if (nstate == 0)
            renderer->DrawUI(this->m_pFont, this->m_pFontMaterial, "Press << 0 >> to Host and << 1 >> to Join", PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
        else if (nstate == 1)
            renderer->DrawUI(this->m_pFont, this->m_pFontMaterial, ("IP: " + this->m_pNetwork->m_IP).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
        else if (nstate == 2)
            renderer->DrawUI(this->m_pFont, this->m_pFontMaterial, ("Port: " + this->m_pNetwork->m_Port).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
        ////////////////////////////////////////////////////
        //if (this->gameStart)
        //{
  /*          for (int i = 0; i < this->m_Players.size(); i++)
                this->m_Players[i]->Ship->Draw(a_pGraphics, this->m_Cameras[0]);*/
        //}

        //////////////////// POST SCREEN ////////////////////////////////
        renderer->Set((float)this->textureID, "textureID");
        renderer->Set(PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
        PuRe_Vector3F size = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
        renderer->Render(this->m_Cameras[0], this->m_pPostMaterial, size);
        size.X += a_pGraphics->GetDescription().ResolutionWidth / 2;
        renderer->Render(this->m_Cameras[1], this->m_pPostMaterial, size);
        renderer->End();
        ////////////////////////////////////////////////////

        /*a_pGraphics->Clear(clear);

        PuRe_BoundingBox Screen;
        Screen.m_Position = PuRe_Vector2F(0.0f, 0.0f);
        Screen.m_Size = PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight);
        a_pGraphics->Begin(Screen);
        this->m_pSkyBox->Draw(this->m_pCamera, this->m_pSkyMaterial);
        if (this->gameStart)
        {
        this->m_pPlayerShip->Draw(a_pGraphics, this->m_pCamera);
        for (unsigned int i = 0; i < this->m_Asteroids.size(); i++)
        this->m_Asteroids[i]->Draw(a_pGraphics, this->m_pCamera);
        }
        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
        this->m_Bullets[i]->Draw(a_pGraphics, this->m_pCamera);

        PuRe_Vector3F minipos = PuRe_Vector3F(Screen.m_Size.X - 150.0f, Screen.m_Size.Y - 150.0f, 128.0f);

        PuRe_Vector3F rot = this->m_pCamera->GetRotation();
        rot *= PuRe_DegToRad;
        PuRe_MatrixF rotation = PuRe_QuaternionF(rot).GetMatrix();

        this->m_pMinimap->Draw(a_pGraphics, this->m_pUICamera, this->m_pUIMaterial, minipos, rotation);
        if (this->gameStart)
        {
        PuRe_Vector3F playerpos = TheBrick::OngToPuRe(this->m_pPlayerShip->m_pBody->getTransform().p);
        this->m_pMinimap->DrawPlayer(a_pGraphics, this->m_pUICamera, this->m_pUIMaterial, playerpos, this->m_MapBoundaries, rotation);
        }

        int nstate = this->m_pNetwork->GetState();
        if (nstate == 0)
        this->m_pFont->Draw(this->m_pUICamera, this->m_pFontMaterial, "Press << 0 >> to Host and << 1 >> to Join", PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_Vector3F(32.0f, 32.0f, 0.0f), PuRe_MatrixF::Identity(), 32.0f);
        else if (nstate == 1)
        this->m_pFont->Draw(this->m_pUICamera, this->m_pFontMaterial, ("IP: " + this->m_pNetwork->m_IP).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_Vector3F(32.0f, 32.0f, 0.0f), PuRe_MatrixF::Identity(), 32.0f);
        else if (nstate == 2)
        this->m_pFont->Draw(this->m_pUICamera, this->m_pFontMaterial, ("Port: " + this->m_pNetwork->m_Port).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_Vector3F(32.0f, 32.0f, 0.0f), PuRe_MatrixF::Identity(), 32.0f);


        a_pGraphics->End();*/


        /*   this->m_pRenderer->Draw(this->m_pModel, PuRe_Primitive::Triangles, this->m_pMaterial, PuRe_Vector3F(0.0f,-1.0f,15.0f),PuRe_Vector3F(),PuRe_Vector3F(),PuRe_Vector3F(10.0f,10.0f,10.0f));
           this->m_pRenderer->Draw(this->m_pModel, PuRe_Primitive::Triangles, this->m_pMaterial, pos + this->m_pCamera->GetForward()*10.0f);
           this->m_pRenderer->Draw(this->m_pPointLight,this->m_pPointLightMaterial,pos,PuRe_Vector3F(1.0f,0.0f,0.0f),1.0f,0.01f,0.01f);
           this->m_pRenderer->Draw(this->m_pSkyBox,this->m_pSkyMaterial);
           this->m_pRenderer->Begin(clear);
           this->m_pRenderer->Render(this->m_pCamera,this->m_pPostMaterial);
           this->m_pRenderer->End();*/

    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Exit()
    {
        //Send to Host that we left
        LeftPacket lPacket;
        lPacket.Head.Type = 1;
        lPacket.Who = this->m_ID;
        this->m_pNetwork->SendHost((char*)&lPacket, sizeof(LeftPacket));
        //Clear Memory
        for (int i = 0; i < this->m_Players.size(); i++)
        {
            SAFE_DELETE(this->m_Players[i]->Ship);
            SAFE_DELETE(this->m_Players[i]);
        }
        this->m_Players.clear();
        // DELETE MATERIALS
        SAFE_DELETE(this->m_pDirectionalLightMaterial);
        SAFE_DELETE(this->m_pPointLightMaterial);
        SAFE_DELETE(this->m_pFontMaterial);
        SAFE_DELETE(this->m_pPostMaterial);
        SAFE_DELETE(this->m_pSkyMaterial);
        SAFE_DELETE(this->m_pUIMaterial);
        SAFE_DELETE(this->m_pMaterial);
        // DELETE OBJECTS
        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
            SAFE_DELETE(this->m_Bullets[i]);
        for (unsigned int i = 0; i < this->m_Asteroids.size(); i++)
            SAFE_DELETE(this->m_Asteroids[i]);
        SAFE_DELETE(this->m_pSkyBox);
        SAFE_DELETE(this->m_pPointLight);
        SAFE_DELETE(this->m_pDirectionalLight);
        // DELETE CAMERAS
        for (unsigned int i = 0; i < this->m_Cameras.size(); i++)
            SAFE_DELETE(this->m_Cameras[i]);
        // DELETE MODELS
        SAFE_DELETE(this->m_pModel);
        // DELETE RENDERER
        SAFE_DELETE(this->m_pMinimap);
        SAFE_DELETE(this->m_pFont);
    }
}