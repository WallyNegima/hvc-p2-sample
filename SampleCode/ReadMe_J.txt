----------------------------------------------------
 B5T-007001 �T���v���R�[�h
----------------------------------------------------

(1) �T���v���R�[�h���e
  �{�T���v���ł́u���o�����v,�u�o�^�����v,�u��F�؏����v��3��ނ̃T���v���R�[�h��p�ӂ��Ă��܂��B

  ���o�����ł�B5T-007001�̊�F�؂�����9�@�\�����s���A���̌��ʂ�W���o�͂ɏo�͂��Ă��܂��B
  �o�^�����ł�B5T-007001�̊�F�؃f�[�^�o�^�@�\�����s���A���̌��ʂ�W���o�͂ɏo�͂��Ă��܂��B
  ��F�؏�����B5T-007001�Ŋ�F�؂����s���A���̌��ʂ�W���o�͂ɏo�͂��Ă��܂��B

(2) �f�B���N�g���\��
      src/
        HVCApi/                     B5T-007001�C���^�[�t�F�[�X�֐�
          HVCApi.c                    API�֐�
          HVCApi.h                    API�֐���`
          HVCDef.h                    �\���̒�`
          HVCExtraUartFunc.h          API�֐�����Ăяo���O���֐���`
        bmp/                        �r�b�g�}�b�v�t�@�C���ۑ��֐�
          bitmap.c                    B5T-007001����擾�����摜���r�b�g�}�b�v�t�@�C���ɕۑ�����֐�
        uart/                       UART�C���^�[�t�F�[�X�֐�
          uart.c                      Windows�œ��삷��UART�֐�
          uart.h                      UART�֐���`
        Album/                      �A���o���t�@�C���ۑ�/�Ǎ��֐�
          Album.c                     B5T-007001����擾�����A���o����I/O���s���֐�
        Sample/                     ���o�����T���v��
          main.c                      �T���v���R�[�h
        FR_Sample/                  �o�^����/��F�؏����T���v��
          register_main.c             �o�^�����T���v���R�[�h
          FR_main.c                   ��F�؏����T���v���R�[�h

(3) �T���v���R�[�h�̃r���h���@
  1. �T���v���R�[�h��Windows7��œ��삷��悤�쐬���Ă��܂��B
     VC10(Visual Studio 2010 C++)��VC14(Visual Studio 2015 C++)�ŃR���p�C���A�����N���\�ł��B
  2. ���o�����T���v��
     HVCApi�f�B���N�g���ȉ��̑S�Ẵt�@�C���ƁAbmp,uart,Sample�f�B���N�g���ȉ���
     �S�Ẵt�@�C���݂̂Ō��o�������s���W���[�����쐬�\�ł��B
  3. �o�^�����T���v��
     HVCApi�f�B���N�g���ȉ��̑S�Ẵt�@�C���ƁAbmp,uart,Album�f�B���N�g���ȉ���
     �S�Ẵt�@�C���AFR_Sample/register_main.c�œo�^�������s���W���[�����쐬�\�ł��B
  4. ��F�؏����T���v��
     HVCApi�f�B���N�g���ȉ��̑S�Ẵt�@�C���ƁAbmp,uart,Album�f�B���N�g���ȉ���
     �S�Ẵt�@�C���AFR_Sample/FR_main.c�Ŋ�F�؏������s���W���[�����쐬�\�ł��B
  
(4) �T���v���R�[�h�̎��s���@
  �{�T���v���R�[�h�̎��s�����邽�߂ɂ́AB5T-007001���ڑ����Ă���uCOM�ԍ��v��UART�́u�{�[���[�g�v��
  �N�������Ŏw�肷��K�v������܂��B

  1. ���o�����T���v��
     sample.exe "COM�ԍ�" "�{�[���[�g"
     ��)sample.exe 1 921600
     
  2. �o�^�����T���v��
     register.exe "COM�ԍ�" "�{�[���[�g"
     ��)register.exe 1 921600
     
  3. ��F�؏����T���v��
     FRsample.exe "COM�ԍ�" "�{�[���[�g"
     ��)FRsample.exe 1 921600

(5) Windows�ȊO�̊��ւ̈ڐA�ɂ���
  1. HVCApi�ɂ���
     ���̃f�B���N�g���ȉ��̃R�[�h�͔ėp�����l���č쐬���Ă��܂��B
     C������R���p�C���ł�����ł���΂��̂܂܂Ŏg�p���Ă��������܂��B
  2. bitmap.c
     ���ɍ��킹�ăr�b�g�}�b�v�t�@�C���ۑ��֐����������Ă��������B
     Windows�̕W��API�֐��ACreateFile()�AWriteFile()�A�����CloseHandle()���g�p���Ă��܂��B
  3. uart.c�Auart.h
     ���ɍ��킹��UART�C���^�[�t�F�[�X�֐����������Ă��������B
     UART�̏������AUART���M�AUART��M�A�����UART�̏I�������ō\������Ă��܂��B
  4. main.c�Aregister_main.c�AFR_main.c
     �L�[�{�[�h���͂�Windows�p�̊֐����g�p���Ă��܂��B
       _kbhit()�F�L�[�{�[�h���̗͂L���𒲂ׂ܂��B
       _getch()�F�R���\�[�����當����ǂݍ��݂܂��B
     ���ɍ��킹�Ă����̊֐��������ւ��Ă��������B

[���g�p�ɂ�������]
�E�{�T���v���R�[�h����уh�L�������g�̒��쌠�̓I�������ɋA�����܂��B
�E�{�T���v���R�[�h�͓����ۏ؂�����̂ł͂���܂���B
�E�{�T���v���R�[�h�́AApache License 2.0�ɂĒ񋟂��Ă��܂��B

----
�I�������������
AOB���ƕ� ���i�J����
Copyright(C) 2014-2017 OMRON Corporation, All Rights Reserved.
