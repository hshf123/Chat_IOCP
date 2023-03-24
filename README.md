# IOCP 채팅 프로그램

C++ IOCP 서버로 간단한 채팅 프로그램을 만들었습니다.<br><br>
서버는 C++ IOCP 서버로 제작했고 클라이언트의 경우 C#과 C++ 두 가지 버전으로 만들었습니다.<br><br>
두 클라이언트는 동일하게 동작하며 C#과 C++ 연동 테스트를 위해 두 가지 버전으로 구성해 보았습니다.<br><br>
따라서 유니티, 언리얼로 만들어진 클라이언트와 IOCP서버를 연동하여 컨텐츠를 제작할 수 있을 것으로 기대하고 있습니다!

## 사용 방법

이 프로젝트는 Visual Studio 2022환경에서 제작 되었습니다.<br><br>
코드를 내려받고 빌드 후 실행하면 됩니다.<br><br>
실행하면 다음과 같은 문구가 등장합니다.<br><br>
![image](https://user-images.githubusercontent.com/66163506/227478553-e075325f-066d-458d-842a-ebf2c6f78b76.png)<br>
이름을 입력하면 서버에서 DB를 확인하고 처음 등장한 이름이라면 새롭게 계정을 생성합니다.<br><br>
![image](https://user-images.githubusercontent.com/66163506/227478701-4c8ab91d-a484-483f-b94a-fdf8f79e83ea.png)<br>
이미 존재하는 계정이라면 DB에서 플레이어 정보를 긁어옵니다.<br><br>
![image](https://user-images.githubusercontent.com/66163506/227479811-6607fe17-5654-40a9-89d7-bce8858bfc0c.png)<br>
하지만 이미 접속중인 계정이라면 서버와의 연결이 강제로 끊기게 됩니다.<br><br>
![image](https://user-images.githubusercontent.com/66163506/227478957-d0bca988-5770-4e1f-83dd-ae7d74784093.png)<br>
후에 방을 선택하고 방에 들어가면 바로 채팅할 수 있습니다.<br><br>
채팅을 입력하면 *이름 : 내용* 형식으로 출력됩니다.<br><br>
![image](https://user-images.githubusercontent.com/66163506/227480309-edfd1a61-e58b-48f2-9a04-c4211914235a.png)
![image](https://user-images.githubusercontent.com/66163506/227480640-1fb8af98-7363-49bf-8f1a-29d94f3624d6.png)<br>
`/exit` 를 입력하여 방에서 나갈 수 있습니다.<br><br>
![image](https://user-images.githubusercontent.com/66163506/227480888-5928eaaa-bc3e-4164-a88a-c0c54a29abfb.png)
![image](https://user-images.githubusercontent.com/66163506/227480914-aab759f3-9f3b-4d82-a45b-0aaf8be1adb0.png)<br>
## 프로젝트 설명

### Client

추후에 현재 서버외에 DB서버 등 여러개의 서버와 연결될 수 있으니 Client에 SessionManager를 두어 연결된 Session을 관리하게끔 하였습니다.<br><br>
방에 대한 정보가 현재는 별로 없어 Managers클래스에서 사용자 정보와 방 ID등 모든 정보를 관리하고 있지만 후에 다른 컨텐츠가 추가 될 경우 방에대한 정보도 따로 분리하여 관리할 계획입니다.<br><br>
채팅 중에도 새로운 유저가 입장하고 퇴장하는 과정에서 콘솔창을 초기화 하는 부분이 있기 때문에 채팅내용을 저장하고 이전 채팅 내용을 볼 수 있도록 하였습니다.<br><br>

### Server

서버는 처음 시작과 동시에 5개의 방을 만들도록 하였습니다.<br><br>
Room 클래스는 방 하나를 의미하고 방에는 여러 명의 사용자 정보를 hash map구조로 저장하고 있습니다.<br><br>
사용자가 처음 생길 때 고유한 ID를 발급받게 되어 이를 key값으로 활용했습니다.<br><br>
사용자로 부터 채팅 패킷을 받으면 플레이어가 소속된 방에 다시 받은 내용을 Broadcast하는 방식으로 동작합니다.<br><br>
여러 방을 관리하는 RoomManager는 전역으로 하나만 존재하여 실제로 방을 찾거나 관리할 때는 RoomManager를 통해 접근하게 됩니다.<br><br>
각 방도 고유한 ID가 존재하여 마찬가지로 RoomManager에서 hash map구조로 방에대한 정보를 저장하고 있습니다.<br><br>
현재는 DB를 사용하는게 무색할정도로 관리하는 정보가 없지만 후에 여러 컨텐츠가 생겨남에따라 관리할 데이터도 많아지면 제 역할을 할 것이라 생각됩니다.<br><br>
또한 서버에서 DB까지 함께 관리하고 있지만 이를 분리하여 DB서버를 따로 구현하는 것 또한 추가할 예정입니다.<br><br>
