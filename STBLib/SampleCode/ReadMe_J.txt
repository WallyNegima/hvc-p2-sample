----------------------------------------------------
 B5T-007001 �T���v���R�[�h (STBLib���p��)
----------------------------------------------------

(1) �T���v���R�[�h���e
  �{�T���v���ł́u���o�����v,�u�o�^�����v,�u��F�؏����v��3��ނ̃T���v���R�[�h��p�ӂ��Ă��܂��B

  ���o�����ł�B5T-007001�̊�F�؂�����9�@�\�����s���A���̌��ʂ�W���o�͂ɏo�͂��Ă��܂��B
  �o�^�����ł�B5T-007001�̊�F�؃f�[�^�o�^�@�\�����s���A���̌��ʂ�W���o�͂ɏo�͂��Ă��܂��B
  ��F�؏�����B5T-007001�Ŋ�F�؂����s���A���̌��ʂ�W���o�͂ɏo�͂��Ă��܂��B

  * �{�T���v���́A���Ђ�荇�킹�Ē񋟂���STBLib�𗘗p���Ȃ�SampleCode���x�[�X�ɉ��ǂ����������̂ł��B
  * ���ʁE�N��E��F�؂ɑ΂��Ĉ��艻���������{���Ă��܂��B
  
(2) �f�B���N�g���\��
      bin/                          �r���h���̏o�̓f�B���N�g��
      platform/                     VC10/VC14�ł̃r���h��
      src/
        HVCApi/                     B5T-007001�C���^�[�t�F�[�X�֐�
          HVCApi.c                    API�֐�
          HVCApi.h                    API�֐���`
          HVCDef.h                    �\���̒�`
          HVCExtraUartFunc.h          API�֐�����Ăяo���O���֐���`
        STBApi/                     STBLib�C���^�[�t�F�[�X�֐�
          STBWrap.c                   STBLib���b�p�[�֐�
          STBWrap.h                   STBLib���b�p�[�֐���`
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
      import/                       STBLib�𗘗p���邽�߂̃C���|�[�g�f�B���N�g��

(3) �T���v���R�[�h�̃r���h���@
  1. import/lib��STBLib�̃��C�u�����t�@�C���Aimport/include�փC���N���[�h�w�b�_�[��
     �i�[���Ă��������B
  2. �T���v���R�[�h��Windows7��œ��삷��悤�쐬���Ă��܂��B
     VC10(Visual Studio 2010 C++)��VC14(Visual Studio 2015 C++)�ŃR���p�C���A�����N���\�ł��B
  3. �R���p�C����́Abin/�ȉ��ɐ��������exe�t�@�C���Ɠ����f�B���N�g���ɁASTBLib��DLL�t�@�C����
     �u���Ă��������B
  4. ���o�����T���v��
     HVCApi�f�B���N�g�������STBApi�f�B���N�g���ȉ��̑S�Ẵt�@�C���ƁAbmp,uart,Sample
     �f�B���N�g���ȉ��̑S�Ẵt�@�C���݂̂Ō��o�������s���W���[�����쐬�\�ł��B
  5. �o�^�����T���v��
     HVCApi�f�B���N�g���ȉ��̑S�Ẵt�@�C���ƁAbmp,uart,Album�f�B���N�g���ȉ���
     �S�Ẵt�@�C���AFR_Sample/register_main.c�œo�^�������s���W���[�����쐬�\�ł��B
  6. ��F�؏����T���v��
     HVCApi�f�B���N�g�������STBApi�f�B���N�g���ȉ��ȉ��̑S�Ẵt�@�C���ƁAbmp,uart,Album
     �f�B���N�g���ȉ��̑S�Ẵt�@�C���AFR_Sample/FR_main.c�Ŋ�F�؏������s���W���[�����쐬�\�ł��B

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

[���g�p�ɂ�������]
�E�{�T���v���R�[�h����уh�L�������g�̒��쌠�̓I�������ɋA�����܂��B
�E�{�T���v���R�[�h�͓����ۏ؂�����̂ł͂���܂���B
�E�{�T���v���R�[�h�́AApache License 2.0�ɂĒ񋟂��Ă��܂��B

----
�I�������������
AOB���ƕ� ���i�J����
Copyright(C) 2014-2017 OMRON Corporation, All Rights Reserved.
