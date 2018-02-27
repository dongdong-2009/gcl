using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace gcl.comm
{
    class SdkNative
    {
        //* is dll
        [StructLayout(LayoutKind.Sequential)]
        public struct PsmAttach
        {
            public int reason;
            public int containerId;
            public int sourceUserId;
            public int targetUserID;
            public int tag;
        }

        public struct AddressVqtIntText1
        {
            public int address;
            public string value;
            public int quality;
            public DateTime datetime;
        }

        public struct AddressInt32Vqt
        {
            public int address;
            public int value;
            public int quality;
            public DateTime datetime;
        }

        public struct AddressVqtRealText15
        {
            public int address;
            public string value;
            public int quality;
            public DateTime datetime;
        }

        public struct AddressDoubleVqt
        {
            public int address;
            public double value;
            public int quality;
            public DateTime datetime;
        }

        public struct AddressVqtString128
        {
            public int address;
            public string value;
            public int quality;
            public DateTime datetime;
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void ErrorCallback(int iErrorId);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void MessageCommandCallback([MarshalAs(UnmanagedType.LPStr)] string sCommand, IntPtr sParam, int iParamLength, ref PsmAttach oAttach);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void FileTransferCallback([MarshalAs(UnmanagedType.LPStr)] string sFilePath, ref PsmAttach oAttach);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void RealtimeDataRequestCallback(ref PsmAttach oAttach);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void RealtimeDataPostCallback(int iParamType, IntPtr pData, int iDataLength, int iParamCount, ref PsmAttach oAttach);


        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_init")]
        private static extern int gci_init(int argc, [In, Out] string[] argv);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_cleanup")]
        private static extern int gci_cleanup();

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_terminal_status")]
        private static extern int gci_terminal_status(int iId);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_terminal_info")]
        private static extern string gci_terminal_info(int iId);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_test")]
        private static extern int gci_test(int iTest);


        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_register_error_notify")]
        private static extern int gci_register_error_notify(ErrorCallback callback);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_unregister_error_notify")]
        private static extern int gci_unregister_error_notify(ErrorCallback callback);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_last_error_id")]
        private static extern int gci_last_error_id();

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_error_info")]
        private static extern string gci_error_info(int iId);


        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_register_message_command_notify")]
        private static extern int gci_register_message_command_notify(MessageCommandCallback callback);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_unregister_message_command_notify")]
        private static extern int gci_unregister_message_command_notify(MessageCommandCallback callback);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_message_command_send")]
        private static extern int gci_message_command_send([MarshalAs(UnmanagedType.LPStr)] string sCommand, [MarshalAs(UnmanagedType.LPStr)] string sParam, int iParamLength, IntPtr oAttach);


        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_register_file_transfer_notify")]
        private static extern int gci_register_file_transfer_notify(FileTransferCallback callback);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_unregister_file_transfer_notify")]
        private static extern int gci_unregister_file_transfer_notify(FileTransferCallback callback);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_file_transfer_send")]
        private static extern int gci_file_transfer_send([MarshalAs(UnmanagedType.LPStr)] string sFilePath, IntPtr oAttach);


        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_register_realtime_data_request_notify")]
        private static extern int gci_register_realtime_data_request_notify(RealtimeDataRequestCallback callback);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_unregister_realtime_data_request_notify")]
        private static extern int gci_unregister_realtime_data_request_notify(RealtimeDataRequestCallback callback);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_register_realtime_data_post_notify")]
        private static extern int gci_register_realtime_data_post_notify(RealtimeDataPostCallback callback);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_unregister_realtime_data_post_notify")]
        private static extern int gci_unregister_realtime_data_post_notify(RealtimeDataPostCallback callback);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_realtime_data_request")]
        private static extern int gci_realtime_data_request(IntPtr oAttach);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_realtime_data_post")]
        private static extern int gci_realtime_data_post(int iParamType, IntPtr pData, int iDataLength, int iParamCount, IntPtr oAttach);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_realtime_data_get_array")]
        private static extern int gci_realtime_data_get_array(int iBeginMid, int iMidCount, int iStructType, IntPtr pOutResult, int iOutResultLength, IntPtr oAttach);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_realtime_data_get_list")]
        private static extern int gci_realtime_data_get_list(int iStructType, IntPtr pOutResult, int iOutResultLength, int iStructCount, IntPtr oAttach);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_realtime_data_get")]
        private static extern int gci_realtime_data_get(int iMid, int iStructType, IntPtr pOutResult, int iOutResultLength, IntPtr oAttach);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_realtime_data_set_list")]
        private static extern int gci_realtime_data_set_list(int iStructType, IntPtr pStructData, int iStructDataLength, int iStructCount, IntPtr oAttach);

        [DllImport("gcl_sdk1.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "gci_realtime_data_set")]
        private static extern int gci_realtime_data_set(int iMid, int iStructType, IntPtr pStructData, int iStructDataLength, IntPtr oAttach);

        //* assist
        private static SdkNative.MessageCommandCallback s_messageCommandCallback = new SdkNative.MessageCommandCallback(dealMessageCommand);
        private static SdkNative.FileTransferCallback s_fileTransferCallback = new SdkNative.FileTransferCallback(dealFileTransfer);
        private static SdkNative.RealtimeDataRequestCallback s_realtimeDataRequestCallback = new SdkNative.RealtimeDataRequestCallback(dealRealtimeDataRequest);
        private static SdkNative.RealtimeDataPostCallback s_realtimeDataPostCallback = new SdkNative.RealtimeDataPostCallback(dealRealtimeDataPost);

        public delegate void ErrorDelegate(int iErrorId);
        public delegate void MessageCommandDelegate(string sCommand, string sParam, ref PsmAttach oAttach);
        public delegate void FileTansferDelegate(string sFilePath, ref PsmAttach oAttach);
        public delegate void RealtimeDataRequestDelegate(ref PsmAttach oAttach);
        public delegate void YxReceivedAddressVqtText1Delegate(AddressVqtIntText1[] yxs, ref PsmAttach oAttach);
        public delegate void YxReceivedAddressInt32VqtDelegate(AddressInt32Vqt[] yxs, ref PsmAttach oAttach);
        public delegate void YcReceivedAddressVqtText15Delegate(AddressVqtRealText15[] ycs, ref PsmAttach oAttach);
        public delegate void YcReceivedAddressDoubleVqtDelegate(AddressDoubleVqt[] ycs, ref PsmAttach oAttach);
        public delegate void YwReceivedAddressVqtString128Delegate(AddressVqtString128[] ycs, ref PsmAttach oAttach);

        private static ErrorDelegate s_errorDelegate = null;
        private static MessageCommandDelegate s_messageCommandDelegate = null;
        private static FileTansferDelegate s_fileTansferDelegate = null;
        private static RealtimeDataRequestDelegate s_realtimeDataRequestDelegate = null;
        private static YxReceivedAddressVqtText1Delegate s_yxReceivedAddressVqtText1Delegate = null;
        private static YxReceivedAddressInt32VqtDelegate s_yxReceivedAddressInt32VqtDelegate = null;
        private static YcReceivedAddressVqtText15Delegate s_ycReceivedAddressVqtText15Delegate = null;
        private static YcReceivedAddressDoubleVqtDelegate s_ycReceivedAddressDoubleVqtDelegate = null;
        private static YwReceivedAddressVqtString128Delegate s_ywReceivedAddressVqtString128Delegate = null;


        //注意：此回调是在子线程中运行的
        //设置错误发生时触发的回调，用委托方式
        public static void setErrorDelegate(ErrorDelegate value) { s_errorDelegate = value; }

        //注意：此回调是在子线程中运行的
        //设置接收到远端消息命令的回调，用委托方式
        public static void setMessageCommandDelegate(MessageCommandDelegate value) { s_messageCommandDelegate = value; }

        //注意：此回调是在子线程中运行的
        //设置接收到远端文件的回调，用委托方式
        public static void setFileTansferDelegate(FileTansferDelegate value) { s_fileTansferDelegate = value; }

        //注意：此回调是在子线程中运行的
        //设置接收到远端实时数据（遥信、遥测）请求的回调，用委托方式
        public static void setRealtimeDataRequestDelegate(RealtimeDataRequestDelegate value) { s_realtimeDataRequestDelegate = value; }

        //注意：此回调是在子线程中运行的
        //设置接收到远端实时数据（遥信）送达的回调，用委托方式
        public static void setYxReceivedAddressVqtText1Delegate(YxReceivedAddressVqtText1Delegate value) { s_yxReceivedAddressVqtText1Delegate = value; }

        //注意：此回调是在子线程中运行的
        //设置接收到远端实时数据（遥信）送达的回调，用委托方式
        public static void setYxReceivedAddressInt32VqtDelegate(YxReceivedAddressInt32VqtDelegate value) { s_yxReceivedAddressInt32VqtDelegate = value; }

        //注意：此回调是在子线程中运行的
        //设置接收到远端实时数据（遥测）送达的回调，用委托方式
        public static void setYcReceivedAddressVqtText1Delegate(YcReceivedAddressVqtText15Delegate value) { s_ycReceivedAddressVqtText15Delegate = value; }

        //注意：此回调是在子线程中运行的
        //设置接收到远端实时数据（遥测）送达的回调，用委托方式
        public static void setYcReceivedAddressDoubleVqtDelegate(YcReceivedAddressDoubleVqtDelegate value) { s_ycReceivedAddressDoubleVqtDelegate = value; }

        //注意：此回调是在子线程中运行的
        //设置接收到远端实时数据（遥文）送达的回调，用委托方式
        public static void setYwReceivedAddressVqtString128Delegate(YwReceivedAddressVqtString128Delegate value) { s_ywReceivedAddressVqtString128Delegate = value; }

        private static void dealError(int iErrorId)
        {
            if (s_errorDelegate != null)
            {
                s_errorDelegate(iErrorId);
            }
        }

        private static void dealMessageCommand([MarshalAs(UnmanagedType.LPStr)] string sCommand, IntPtr sParam, int iParamLength, ref PsmAttach oAttach)
        {
            if (s_messageCommandDelegate != null)
            {
                string sParam1 = SdkNative.assignToString(sParam, iParamLength);
                s_messageCommandDelegate(sCommand, sParam1, ref oAttach);
            }
        }

        private static void dealFileTransfer([MarshalAs(UnmanagedType.LPStr)] string sFilePath, ref PsmAttach oAttach)
        {
            if (s_fileTansferDelegate != null)
            {
                s_fileTansferDelegate(sFilePath, ref oAttach);
            }
        }

        private static void dealRealtimeDataRequest(ref PsmAttach oAttach)
        {
            if (s_realtimeDataRequestDelegate != null)
            {
                s_realtimeDataRequestDelegate(ref oAttach);
            }
        }

        private static void dealRealtimeDataPost(int iParamType, IntPtr pData, int iDataLength, int iParamCount, ref PsmAttach oAttach)
        {
            if (iParamType == 0x01010203 && s_yxReceivedAddressInt32VqtDelegate != null && iParamCount > 0 && iParamCount < 1024)
            {
                AddressInt32Vqt[] values = toAddressInt32Vqts(pData, iDataLength, iParamCount);
                s_yxReceivedAddressInt32VqtDelegate(values, ref oAttach);
            }
            else if (iParamType == 0x01010205 && s_yxReceivedAddressVqtText1Delegate != null && iParamCount > 0 && iParamCount < 1024)
            {
                AddressVqtIntText1[] values = toAddressVqtIntText1s(pData, iDataLength, iParamCount);
                s_yxReceivedAddressVqtText1Delegate(values, ref oAttach);
            }
            else if (iParamType == 0x0101021C && s_ycReceivedAddressDoubleVqtDelegate != null && iParamCount > 0 && iParamCount < 1024)
            {
                AddressDoubleVqt[] values = toAddressAddressDoubleVqts(pData, iDataLength, iParamCount);
                s_ycReceivedAddressDoubleVqtDelegate(values, ref oAttach);
            }
            else if (iParamType == 0x0101021E && s_ycReceivedAddressVqtText15Delegate != null && iParamCount > 0 && iParamCount < 1024)
            {
                AddressVqtRealText15[] values = toAddressVqtRealText15s(pData, iDataLength, iParamCount);
                s_ycReceivedAddressVqtText15Delegate(values, ref oAttach);
            }
            else if (iParamType == 0x0101022F && s_ywReceivedAddressVqtString128Delegate != null && iParamCount > 0 && iParamCount < 1024)
            {
                AddressVqtString128[] values = toAddressVqtString128s(pData, iDataLength, iParamCount);
                s_ywReceivedAddressVqtString128Delegate(values, ref oAttach);
            }
        }

        private static AddressVqtIntText1[] toAddressVqtIntText1s(IntPtr pData, int iDataLength, int iParamCount)
        {
            //int sbyte[8] int long == 24
            if (iDataLength < iParamCount * 24)
            {
                return null;
            }
            //byte[] bs = new byte[iDataLength];
            //Marshal.Copy(pData, bs, 0, iDataLength);
            AddressVqtIntText1[] values = new AddressVqtIntText1[iParamCount];
            int iOffset = 0;
            for (int i = 0; i < iParamCount; i++)
            {
                values[i].address = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                values[i].value = assignToString(pData, iOffset, 1); iOffset += 8;
                values[i].quality = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                long dt = Marshal.ReadInt64(pData, iOffset); iOffset += 8;
                if (dt > MinMsepoch && dt < MaxMsepoch)
                {
                    values[i].datetime = msepochToDateTime(dt);
                }
            }
            //ptr = (IntPtr)(((long)ptr) + perf_object_type.TotalByteLength);
            return values;
        }

        private static AddressInt32Vqt[] toAddressInt32Vqts(IntPtr pData, int iDataLength, int iParamCount)
        {
            //int int int long == 20
            if (iDataLength < iParamCount * 20)
            {
                return null;
            }
            //byte[] bs = new byte[iDataLength];
            //Marshal.Copy(pData, bs, 0, iDataLength);
            AddressInt32Vqt[] values = new AddressInt32Vqt[iParamCount];
            int iOffset = 0;
            for (int i = 0; i < iParamCount; i++)
            {
                values[i].address = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                values[i].value = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                values[i].quality = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                long dt = Marshal.ReadInt64(pData, iOffset); iOffset += 8;
                if (dt > MinMsepoch && dt < MaxMsepoch)
                {
                    values[i].datetime = msepochToDateTime(dt);
                }
            }
            //ptr = (IntPtr)(((long)ptr) + perf_object_type.TotalByteLength);
            return values;
        }

        private static AddressVqtRealText15[] toAddressVqtRealText15s(IntPtr pData, int iDataLength, int iParamCount)
        {
            //int sbyte[16] int long == 32
            if (iDataLength < iParamCount * 32)
            {
                return null;
            }
            //byte[] bs = new byte[iDataLength];
            //Marshal.Copy(pData, bs, 0, iDataLength);
            AddressVqtRealText15[] values = new AddressVqtRealText15[iParamCount];
            int iOffset = 0;
            for (int i = 0; i < iParamCount; i++)
            {
                values[i].address = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                values[i].value = assignToString(pData, iOffset, 15); iOffset += 16;
                values[i].quality = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                long dt = Marshal.ReadInt64(pData, iOffset); iOffset += 8;
                if (dt > MinMsepoch && dt < MaxMsepoch)
                {
                    values[i].datetime = msepochToDateTime(dt);
                }
            }
            //ptr = (IntPtr)(((long)ptr) + perf_object_type.TotalByteLength);
            return values;
        }

        private static AddressDoubleVqt[] toAddressAddressDoubleVqts(IntPtr pData, int iDataLength, int iParamCount)
        {
            //int double int long == 24
            if (iDataLength < iParamCount * 24)
            {
                return null;
            }
            //byte[] bs = new byte[iDataLength];
            //Marshal.Copy(pData, bs, 0, iDataLength);
            AddressDoubleVqt[] values = new AddressDoubleVqt[iParamCount];
            long lTemp;
            int iOffset = 0;
            for (int i = 0; i < iParamCount; i++)
            {
                values[i].address = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                lTemp = Marshal.ReadInt64(pData, iOffset); iOffset += 8; values[i].value = BitConverter.Int64BitsToDouble(lTemp);
                values[i].quality = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                long dt = Marshal.ReadInt64(pData, iOffset); iOffset += 8;
                if (dt > MinMsepoch && dt < MaxMsepoch)
                {
                    values[i].datetime = msepochToDateTime(dt);
                }
            }
            //ptr = (IntPtr)(((long)ptr) + perf_object_type.TotalByteLength);
            return values;
        }

        private static AddressVqtString128[] toAddressVqtString128s(IntPtr pData, int iDataLength, int iParamCount)
        {
            //int sbyte[128] int long == 144
            if (iDataLength < iParamCount * 144)
            {
                return null;
            }
            //byte[] bs = new byte[iDataLength];
            //Marshal.Copy(pData, bs, 0, iDataLength);
            AddressVqtString128[] values = new AddressVqtString128[iParamCount];
            int iOffset = 0;
            for (int i = 0; i < iParamCount; i++)
            {
                values[i].address = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                values[i].value = assignToString(pData, iOffset, 127); iOffset += 128;
                values[i].quality = Marshal.ReadInt32(pData, iOffset); iOffset += 4;
                long dt = Marshal.ReadInt64(pData, iOffset); iOffset += 8;
                if (dt > MinMsepoch && dt < MaxMsepoch)
                {
                    values[i].datetime = msepochToDateTime(dt);
                }
            }
            //ptr = (IntPtr)(((long)ptr) + perf_object_type.TotalByteLength);
            return values;
        }


        //SDK初始化
        //注册DLL回调的实现地方
        public static string init()
        {
            StringBuilder r = new StringBuilder();
            string[] sLineArgs = Environment.GetCommandLineArgs();
            int iResult = gci_init(sLineArgs.Length, sLineArgs);
            r.Append("gci_init : " + Convert.ToString(iResult) + "\n");
            if (iResult > 0)
            {
                //message
                iResult = gci_register_message_command_notify(s_messageCommandCallback);
                r.Append("gci_register_message_command_notify : " + Convert.ToString(iResult) + "\n");
                //file
                iResult = gci_register_file_transfer_notify(s_fileTransferCallback);
                r.Append("gci_register_file_transfer_notify : " + Convert.ToString(iResult) + "\n");
                //realtime data request
                iResult = gci_register_realtime_data_request_notify(s_realtimeDataRequestCallback);
                r.Append("gci_register_realtime_data_request_notify : " + Convert.ToString(iResult) + "\n");
                //realtime data post
                iResult = gci_register_realtime_data_post_notify(s_realtimeDataPostCallback);
                r.Append("gci_register_realtime_data_post_notify : " + Convert.ToString(iResult) + "\n");
            }
            return r.ToString();
        }

        //程序退出
        public static void clear()
        {
            gci_cleanup();
        }
        
        //查询与远端的连接状态
        //只有与远端有心跳来回时才为True
        public static bool terminalStatus()
        {
            return gci_terminal_status(0) != 0;
        }

        //查询与远端的连接状态
        //只有与远端有心跳来回时才为True
        public static string terminalInfo()
        {
            return gci_terminal_info(0);
        }

        //发送消息命令到远端，不带有附件参数
        //消息命令：command是\0为结束符的，param是可以包括\0的任意字符
        public static int SendMessageCommand(string sCommand, string sParam)
        {

            byte[] bylen = Encoding.Default.GetBytes(sParam);
            //return gci_message_command_send(sCommand, sParam, sParam.Length, IntPtr.Zero);
            return gci_message_command_send(sCommand, sParam, bylen.Length, IntPtr.Zero);
        }

        //发送消息命令到远端，带有附件参数
        //消息命令：command是\0为结束符的，param是可以包括\0的任意字符
        public static unsafe int SendMessageCommand(string sCommand, string sParam, PsmAttach attach)
        {
            int r = 0;
            IntPtr pnt = Marshal.AllocHGlobal(Marshal.SizeOf(attach));
            try
            {
                // Copy the struct to unmanaged memory.
                byte[] bylen = Encoding.Default.GetBytes(sParam);

                Marshal.StructureToPtr(attach, pnt, false);
                // r = gci_message_command_send(sCommand, sParam, sParam.Length, pnt);
                r = gci_message_command_send(sCommand, sParam, bylen.Length, pnt);
            }
            finally
            {
                // Free the unmanaged memory.
                Marshal.FreeHGlobal(pnt);
            }
            return r;
        }

        ////发送文件到远端，不带有附件参数
        ////消息命令：feilPath是\0为结束符的
        public static int SendFileTransfer(string sFilePath)
        {
            return gci_file_transfer_send(sFilePath, IntPtr.Zero);
        }

        //发送文件到远端，带有附件参数
        //消息命令：feilPath是\0为结束符的
        public static int SendFileTransfer(string sFilePath, PsmAttach attach)
        {
            int r = 0;
            IntPtr pnt = Marshal.AllocHGlobal(Marshal.SizeOf(attach));
            try
            {
                // Copy the struct to unmanaged memory.
                Marshal.StructureToPtr(attach, pnt, false);
                r = gci_file_transfer_send(sFilePath, pnt);
            }
            finally
            {
                // Free the unmanaged memory.
                Marshal.FreeHGlobal(pnt);
            }
            return r;
        }

        //ycs.Length * 32 < 最大帧长 ， 大于要分帧 ， 默认最大帧长为 4000
        public static int SendAddressVqtIntText1s(AddressVqtIntText1[] yxs)
        {
            //byte[] datas = new byte[vqt.Length * 24];
            int r = 0;

            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(yxs.Length * 24);
            int iOffset = 0;
            try
            {
                for (int i = 0; i < yxs.Length; i++)
                {
                    Marshal.WriteInt32(pnt, iOffset, yxs[i].address); iOffset += 4;
                    byte[] value = System.Text.Encoding.Default.GetBytes(yxs[i].value);
                    for (int j = 0; j < 8; j++)
                    {
                        if (j < value.Length)
                        {
                            Marshal.WriteByte(pnt, iOffset, value[j]); iOffset += 1;
                        }
                        else
                        {
                            Marshal.WriteByte(pnt, iOffset, 0); iOffset += 1;
                        }
                    }
                    Marshal.WriteInt32(pnt, iOffset, yxs[i].quality); iOffset += 4;
                    Marshal.WriteInt64(pnt, iOffset, getMsepoch(yxs[i].datetime)); iOffset += 8;
                }

                r = gci_realtime_data_post(0x01010205, pnt, yxs.Length * 24, yxs.Length, IntPtr.Zero);
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }
            return r;
        }

        //yxs.Length * 20 < 最大帧长 ， 大于要分帧 ， 默认最大帧长为 4000
        public static int SendAddressInt32Vqts(AddressInt32Vqt[] yxs)
        {
            //byte[] datas = new byte[vqt.Length * 20];
            int r = 0;

            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(yxs.Length * 20);
            int iOffset = 0;
            try
            {
                for (int i = 0; i < yxs.Length; i++)
                {
                    Marshal.WriteInt32(pnt, iOffset, yxs[i].address); iOffset += 4;
                    Marshal.WriteInt32(pnt, iOffset, yxs[i].value); iOffset += 4;
                    Marshal.WriteInt32(pnt, iOffset, yxs[i].quality); iOffset += 4;
                    Marshal.WriteInt64(pnt, iOffset, getMsepoch( yxs[i].datetime )); iOffset += 8;
                }

                r = gci_realtime_data_post(0x01010203, pnt, yxs.Length * 20, yxs.Length, IntPtr.Zero);
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }
            return r;
        }

        //ycs.Length * 32 < 最大帧长 ， 大于要分帧 ， 默认最大帧长为 4000
        public static int SendAddressVqtRealText15s(AddressVqtRealText15[] ycs)
        {
            //byte[] datas = new byte[vqt.Length * 32];
            int r = 0;

            IntPtr pnt = IntPtr.Zero;

            pnt = Marshal.AllocHGlobal(ycs.Length * 32);

            int iOffset = 0;
            try
            {
                for (int i = 0; i < ycs.Length; i++)
                {
                    Marshal.WriteInt32(pnt, iOffset, ycs[i].address); iOffset += 4;
                    byte[] value = System.Text.Encoding.Default.GetBytes(ycs[i].value);
                    for (int j = 0; j < 16; j++)
                    {
                        if (j < value.Length)
                        {
                            Marshal.WriteByte(pnt, iOffset, value[j]); iOffset += 1;
                        }
                        else
                        {
                            Marshal.WriteByte(pnt, iOffset, 0); iOffset += 1;
                        }
                    }
                    Marshal.WriteInt32(pnt, iOffset, ycs[i].quality); iOffset += 4;
                    Marshal.WriteInt64(pnt, iOffset, getMsepoch( ycs[i].datetime )); iOffset += 8;
                }

                r = gci_realtime_data_post(0x0101021E, pnt, ycs.Length * 32, ycs.Length, IntPtr.Zero);
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }
            return r;
        }

        //ycs.Length * 24 < 最大帧长 ， 大于要分帧 ， 默认最大帧长为 4000
        public static int SendAddressDoubleVqts(AddressDoubleVqt[] ycs)
        {
            //byte[] datas = new byte[vqt.Length * 24];
            int r = 0;

            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(ycs.Length * 24);
            int iOffset = 0;
            try
            {
                long lTemp;
                for (int i = 0; i < ycs.Length; i++)
                {
                    Marshal.WriteInt32(pnt, iOffset, ycs[i].address); iOffset += 4;
                    lTemp = BitConverter.DoubleToInt64Bits(ycs[i].value);
                    Marshal.WriteInt64(pnt, iOffset, lTemp); iOffset += 8;
                    Marshal.WriteInt32(pnt, iOffset, ycs[i].quality); iOffset += 4;
                    Marshal.WriteInt64(pnt, iOffset, getMsepoch( ycs[i].datetime )); iOffset += 8;
                }

                r = gci_realtime_data_post(0x0101021C, pnt, ycs.Length * 24, ycs.Length, IntPtr.Zero);
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }
            return r;
        }

        //yws.Length * 144 < 最大帧长 ， 大于要分帧 ， 默认最大帧长为 4000
        public static int SendAddressVqtString128s(AddressVqtString128[] yws)
        {
            //byte[] datas = new byte[vqt.Length * 144];
            int r = 0;

            IntPtr pnt = IntPtr.Zero;

            pnt = Marshal.AllocHGlobal(yws.Length * 144);

            int iOffset = 0;
            try
            {
                for (int i = 0; i < yws.Length; i++)
                {
                    Marshal.WriteInt32(pnt, iOffset, yws[i].address); iOffset += 4;
                    byte[] value = System.Text.Encoding.UTF8.GetBytes(yws[i].value);
                    for (int j = 0; j < 128; j++)
                    {
                        if (j < value.Length)
                        {
                            Marshal.WriteByte(pnt, iOffset, value[j]); iOffset += 1;
                        }
                        else
                        {
                            Marshal.WriteByte(pnt, iOffset, 0); iOffset += 1;
                        }
                    }
                    Marshal.WriteInt32(pnt, iOffset, yws[i].quality); iOffset += 4;
                    Marshal.WriteInt64(pnt, iOffset, getMsepoch( yws[i].datetime )); iOffset += 8;
                }

                r = gci_realtime_data_post(0x0101022F, pnt, yws.Length * 144, yws.Length, IntPtr.Zero);
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }
            return r;
        }

        //c#版gcl_sdk已经增加了9个读共享内存的函数，分别有以数组array，散列list，单个的方式
        //接口如下（详细看gcl_comm_sdk.cs）：
        //GetAddressInt32VqtArray
        //GetAddressDoubleVqtArray
        //GetAddressVqtString128Array
        //GetAddressInt32VqtList
        //GetAddressDoubleVqtList
        //GetAddressVqtString128List
        //GetAddressInt32Vqt
        //GetAddressDoubleVqt
        //GetAddressVqtString128
        //测试程序在QT版本的SDK_TOOL中

        //new at 20160510 by oudream
        //读实时数据以数组方式：遥信、开关量、状态
        //* @param iBeginMid ： MeasureId （起始ID）
        //* @param iMidCount ：要读的数量
        //* @return null：读取失败；
        public static AddressInt32Vqt[] GetAddressInt32VqtArray(int iBeginMid, int iMidCount)
        {
            //byte[] datas = new byte[vqt.Length * 20];

            int iDataLength = iMidCount * 20;
            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(iDataLength);
            AddressInt32Vqt[] yxs = null;
            try
            {
                int r = gci_realtime_data_get_array(iBeginMid, iMidCount, 0x01010203, pnt, iDataLength, IntPtr.Zero);
                if (r > 0)
                {
                    yxs = toAddressInt32Vqts(pnt, iDataLength, iMidCount);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }

            return yxs;
        }

        //new at 20160510 by oudream
        //读实时数据以数组方式：遥测、模拟量
        //* @param iBeginMid ： MeasureId （起始ID）
        //* @param iMidCount ：要读的数量
        //* @return null：读取失败；
        public static AddressDoubleVqt[] GetAddressDoubleVqtArray(int iBeginMid, int iMidCount)
        {
            //byte[] datas = new byte[vqt.Length * 24];

            int iDataLength = iMidCount * 24;
            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(iDataLength);
            AddressDoubleVqt[] ycs = null;
            try
            {
                int r = gci_realtime_data_get_array(iBeginMid, iMidCount, 0x0101021C, pnt, iDataLength, IntPtr.Zero);
                if (r > 0)
                {
                    ycs = toAddressAddressDoubleVqts(pnt, iDataLength, iMidCount);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }

            return ycs;
        }

        //new at 20160510 by oudream
        //读实时数据以数组方式：遥文
        //*实时数据读（以数组array，散列list，单个的方式）目前支持结构体类型：遥信（gcd_address_int32_vqt_t）， 遥测（gcd_address_double_vqt_t）， 遥文（gcd_firstAddress_vqt_string128_t）
        //* @param iBeginMid ： MeasureId （起始ID）
        //* @param iMidCount ：要读的数量
        //* @return null：读取失败；
        public static AddressVqtString128[] GetAddressVqtString128Array(int iBeginMid, int iMidCount)
        {
            //byte[] datas = new byte[vqt.Length * 144];

            int iDataLength = iMidCount * 144;
            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(iDataLength);
            AddressVqtString128[] yws = null;
            try
            {
                int r = gci_realtime_data_get_array(iBeginMid, iMidCount, 0x0101022F, pnt, iDataLength, IntPtr.Zero);
                if (r > 0)
                {
                    yws = toAddressVqtString128s(pnt, iDataLength, iMidCount);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }

            return yws;
        }


        //new at 20160511 by oudream
        //读实时数据以散列方式：遥信、开关量、状态
        //* @param inOutYxs ：输入与输出结构体，输入时存放有Mid字段值，即 address；输出时带有查询结果数据信息
        //* @return ： 0：什么都没发生，函数空白； >0 ：表示成功读了多少个；； <0 ：表示错误；
        public static int GetAddressInt32VqtList(ref AddressInt32Vqt[] inOutYxs)
        {
            //byte[] datas = new byte[vqt.Length * 20];

            int r = 0;
            int iMidCount = inOutYxs.Length;
            if (iMidCount <= 0) return r;
            int iDataLength = iMidCount * 20;
            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(iDataLength);
            int iOffset = 0;
            try
            {
                for (int i = 0; i < inOutYxs.Length; i++)
                {
                    Marshal.WriteInt32(pnt, iOffset, inOutYxs[i].address); iOffset += 4;
                    Marshal.WriteInt32(pnt, iOffset, inOutYxs[i].value); iOffset += 4;
                    Marshal.WriteInt32(pnt, iOffset, inOutYxs[i].quality); iOffset += 4;
                    Marshal.WriteInt64(pnt, iOffset, getMsepoch( inOutYxs[i].datetime )); iOffset += 8;
                }
                r = gci_realtime_data_get_list(0x01010203, pnt, iDataLength, iMidCount, IntPtr.Zero);
                if (r > 0)
                {
                    inOutYxs = toAddressInt32Vqts(pnt, iDataLength, iMidCount);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }

            return r;
        }

        //new at 20160510 by oudream
        //读实时数据以散列方式：遥测、模拟量
        //* @param inOutYxs ：输入与输出结构体，输入时存放有Mid字段值，即 address；输出时带有查询结果数据信息
        //* @return ： 0：什么都没发生，函数空白； >0 ：表示成功读了多少个；； <0 ：表示错误；
        public static int GetAddressDoubleVqtList(ref AddressDoubleVqt[] inOutYcs)
        {
            //byte[] datas = new byte[vqt.Length * 24];

            int r = 0;
            int iMidCount = inOutYcs.Length;
            if (iMidCount <= 0) return r;
            int iDataLength = iMidCount * 24;
            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(iDataLength);
            int iOffset = 0;
            try
            {
                long lTemp;
                for (int i = 0; i < inOutYcs.Length; i++)
                {
                    Marshal.WriteInt32(pnt, iOffset, inOutYcs[i].address); iOffset += 4;
                    lTemp = BitConverter.DoubleToInt64Bits(inOutYcs[i].value);
                    Marshal.WriteInt64(pnt, iOffset, lTemp); iOffset += 8;
                    Marshal.WriteInt32(pnt, iOffset, inOutYcs[i].quality); iOffset += 4;
                    Marshal.WriteInt64(pnt, iOffset, getMsepoch( inOutYcs[i].datetime )); iOffset += 8;
                }
                r = gci_realtime_data_get_list(0x0101021C, pnt, iDataLength, iMidCount, IntPtr.Zero);
                if (r > 0)
                {
                    inOutYcs = toAddressAddressDoubleVqts(pnt, iDataLength, iMidCount);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }

            return r;
        }

        //new at 20160510 by oudream
        //读实时数据以散列方式：遥文
        //* @param inOutYxs ：输入与输出结构体，输入时存放有Mid字段值，即 address；输出时带有查询结果数据信息
        //* @return ： 0：什么都没发生，函数空白； >0 ：表示成功读了多少个；； <0 ：表示错误；
        public static int GetAddressVqtString128List(ref AddressVqtString128[] yws)
        {
            //byte[] datas = new byte[vqt.Length * 144];

            int r = 0;
            int iMidCount = yws.Length;
            if (iMidCount <= 0) return r;
            int iDataLength = iMidCount * 144;
            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(iDataLength);
            int iOffset = 0;
            try
            {
                for (int i = 0; i < yws.Length; i++)
                {
                    Marshal.WriteInt32(pnt, iOffset, yws[i].address); iOffset += 4;
                    byte[] value = System.Text.Encoding.UTF8.GetBytes(yws[i].value);
                    for (int j = 0; j < 128; j++)
                    {
                        if (j < value.Length)
                        {
                            Marshal.WriteByte(pnt, iOffset, value[j]); iOffset += 1;
                        }
                        else
                        {
                            Marshal.WriteByte(pnt, iOffset, 0); iOffset += 1;
                        }
                    }
                    Marshal.WriteInt32(pnt, iOffset, yws[i].quality); iOffset += 4;
                    Marshal.WriteInt64(pnt, iOffset, getMsepoch( yws[i].datetime )); iOffset += 8;
                }
                r = gci_realtime_data_get_list(0x0101022F, pnt, iDataLength, iMidCount, IntPtr.Zero);
                if (r > 0)
                {
                    yws = toAddressVqtString128s(pnt, iDataLength, iMidCount);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }

            return r;
        }

        //new at 20160511 by oudream
        //读实时数据以散列方式：遥信、开关量、状态
        //* @param inOutYx ：输入与输出结构体，输入时存放有Mid字段值，即 address；输出时带有查询结果数据信息
        //* @return ： 0：什么都没发生，函数空白； >0 ：表示成功读了多少个；； <0 ：表示错误；
        public static int GetAddressInt32Vqt(ref AddressInt32Vqt inOutYx)
        {
            //byte[] datas = new byte[vqt.Length * 20];

            int r = 0;
            int iMidCount = 1;
            int iDataLength = iMidCount * 20;
            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(iDataLength);
            int iOffset = 0;
            try
            {
                Marshal.WriteInt32(pnt, iOffset, inOutYx.address); iOffset += 4;
                Marshal.WriteInt32(pnt, iOffset, inOutYx.value); iOffset += 4;
                Marshal.WriteInt32(pnt, iOffset, inOutYx.quality); iOffset += 4;
                Marshal.WriteInt64(pnt, iOffset, getMsepoch( inOutYx.datetime )); iOffset += 8;
                r = gci_realtime_data_get_list(0x01010203, pnt, iDataLength, iMidCount, IntPtr.Zero);
                if (r > 0)
                {
                    AddressInt32Vqt[] yxs = toAddressInt32Vqts(pnt, iDataLength, iMidCount);
                    if (yxs != null && yxs.Length > 0) inOutYx = yxs[0];
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }

            return r;
        }

        //new at 20160510 by oudream
        //读实时数据以散列方式：遥测、模拟量
        //* @param inOutYc ：输入与输出结构体，输入时存放有Mid字段值，即 address；输出时带有查询结果数据信息
        //* @return ： 0：什么都没发生，函数空白； >0 ：表示成功读了多少个；； <0 ：表示错误；
        public static int GetAddressDoubleVqt(ref AddressDoubleVqt inOutYc)
        {
            //byte[] datas = new byte[vqt.Length * 24];

            int r = 0;
            int iMidCount = 1;
            int iDataLength = iMidCount * 24;
            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(iDataLength);
            int iOffset = 0;
            try
            {
                long lTemp;
                Marshal.WriteInt32(pnt, iOffset, inOutYc.address); iOffset += 4;
                lTemp = BitConverter.DoubleToInt64Bits(inOutYc.value);
                Marshal.WriteInt64(pnt, iOffset, lTemp); iOffset += 8;
                Marshal.WriteInt32(pnt, iOffset, inOutYc.quality); iOffset += 4;
                Marshal.WriteInt64(pnt, iOffset, getMsepoch( inOutYc.datetime )); iOffset += 8;
                r = gci_realtime_data_get_list(0x0101021C, pnt, iDataLength, iMidCount, IntPtr.Zero);
                if (r > 0)
                {
                    AddressDoubleVqt[] ycs = toAddressAddressDoubleVqts(pnt, iDataLength, iMidCount);
                    if (ycs != null && ycs.Length > 0) inOutYc = ycs[0];
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }

            return r;
        }

        //new at 20160510 by oudream
        //读实时数据以散列方式：遥文
        //* @param inOutYw ：输入与输出结构体，输入时存放有Mid字段值，即 address；输出时带有查询结果数据信息
        //* @return ： 0：什么都没发生，函数空白； >0 ：表示成功读了多少个；； <0 ：表示错误；
        public static int GetAddressVqtString128(ref AddressVqtString128 inOutYw)
        {
            //byte[] datas = new byte[vqt.Length * 144];

            int r = 0;
            int iMidCount = 1;
            int iDataLength = iMidCount * 144;
            IntPtr pnt = IntPtr.Zero;
            pnt = Marshal.AllocHGlobal(iDataLength);
            int iOffset = 0;
            try
            {
                Marshal.WriteInt32(pnt, iOffset, inOutYw.address); iOffset += 4;
                byte[] value = System.Text.Encoding.UTF8.GetBytes(inOutYw.value);
                for (int j = 0; j < 128; j++)
                {
                    if (j < value.Length)
                    {
                        Marshal.WriteByte(pnt, iOffset, value[j]); iOffset += 1;
                    }
                    else
                    {
                        Marshal.WriteByte(pnt, iOffset, 0); iOffset += 1;
                    }
                }
                Marshal.WriteInt32(pnt, iOffset, inOutYw.quality); iOffset += 4;
                Marshal.WriteInt64(pnt, iOffset, getMsepoch( inOutYw.datetime )); iOffset += 8;
                r = gci_realtime_data_get_list(0x0101022F, pnt, iDataLength, iMidCount, IntPtr.Zero);
                if (r > 0)
                {
                    AddressVqtString128[] yws = toAddressVqtString128s(pnt, iDataLength, iMidCount);
                    if (yws != null && yws.Length > 0) inOutYw = yws[0];
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pnt);
            }

            return r;
        }


        //* not is dll
        private static unsafe string assignToString(IntPtr sParam, int iParamLength)
        {
            if (iParamLength > 0 && iParamLength < 4096)
            {
                UTF8Encoding encoding = new UTF8Encoding(true, true);
                try
                {
                    return new string((sbyte*)sParam, 0, iParamLength, encoding);
                }
                catch (Exception ex)
                {

                }
            }
            return "";
        }

        private static unsafe string assignToString(IntPtr sParam, int ofs, int iParamLength)
        {
            if (iParamLength > 0 && iParamLength < 4096)
            {
                UTF8Encoding encoding = new UTF8Encoding(true, true);
                try
                {
                    return new string((sbyte*)sParam, ofs, iParamLength, encoding);
                }
                catch (Exception ex)
                {

                }
            }
            return "";
        }

        private static string joinToString(string[] sLines, char cJoin)
        {
            StringBuilder r = new StringBuilder();
            for (int i = 0; i < sLines.Length; i++)
            {
                r.Append(sLines[i]);
                r.Append(cJoin);
            }
            return r.ToString();
        }

        public static string joinToString(string[] sLines)
        {
            char cJoin = '\n';
            return joinToString(sLines, cJoin);
        }

        public static readonly DateTime OriginalUnix = new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc);
        public const Int64 MaxMsepoch = 253402271999000;
        public const Int64 MinMsepoch = -2177481600000;

        public static Int64 currentMsepoch()
        {
            return (Int64)DateTime.UtcNow.Subtract(OriginalUnix).TotalMilliseconds;
        }

        public static int currentSepoch()
        {
            return (int)DateTime.UtcNow.Subtract(OriginalUnix).TotalSeconds;
        }

        public static Int64 getMsepoch(DateTime dt)
        {
            return (Int64)dt.Subtract(OriginalUnix).TotalMilliseconds;
        }

        public static int getSepoch(DateTime dt)
        {
            return (int)dt.Subtract(OriginalUnix).TotalSeconds;
        }

        public static DateTime msepochToDateTime(Int64 dt)
        {
            return OriginalUnix.AddMilliseconds(dt);
        }

        public static DateTime sepochToDateTime(int dt)
        {
            return OriginalUnix.AddSeconds(dt);
        }

    }
}
