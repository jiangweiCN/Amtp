using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using AmtpWinTester.Properties;

namespace AmtpWinTester
{
    public partial class AmtpcaMainForm : System.Windows.Forms.Form
    {
        private Settings settings = new Settings();
        const string dll_name = "Amtpca.dll";
        private delegate void LibCmdRecvCallBack(UInt32 cmd, IntPtr cmd_s, IntPtr reserve);
        private static LibCmdRecvCallBack callback;

        [DllImport(dll_name, EntryPoint = "amtpca_version", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Winapi)]
        private static extern IntPtr AmtpcaVersion();
        [DllImport(dll_name, EntryPoint = "amtpca_init")]
        private static extern unsafe int AmtpcaInit(string license_path, string log_path, LibCmdRecvCallBack callback);
        [DllImport(dll_name, EntryPoint = "amtpca_release")]
        private static extern void AmtpcaRelease();
        [DllImport(dll_name, EntryPoint = "amtpca_sendcmd")]
        private static extern void AmtpcaSendCmd(UInt32 cmd, IntPtr cmd_s, bool sync, UInt32 timeout);
        [DllImport(dll_name, EntryPoint = "amtpca_waitforcmd")]
        private static extern Int32 AmtpcaWaitForCmd(UInt32 cmd, IntPtr cmd_s, UInt32 timeout);
        //AMTPCA_DLL int __stdcall amtpca_waitforcmd(uint32_t cmd, void* s, uint32_t timeout);

        bool init = false;
        DllResult dll_result = new DllResult();
        public AmtpcaMainForm()
        {
            InitializeComponent();
        }

        private void AmtpcaMainForm_Load(object sender, EventArgs e)
        {
            IntPtr intPtr = AmtpcaVersion();
            if (intPtr != IntPtr.Zero)
            {
                string dll_version = Marshal.PtrToStringAnsi(intPtr);
                string text = this.Text + "  " + dll_version;
                this.Text = text;
            }
            this.licTextBox.Text = settings.licFilePath;
            this.dataFileTextBox.Text = settings.ulDataFilePath;
            this.logCheckbox.Checked = settings.outputLog;
        }
        private void AmtpcaMainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            settings.licFilePath = this.licTextBox.Text;
            settings.ulDataFilePath = this.dataFileTextBox.Text;
            settings.outputLog = this.logCheckbox.Checked;
            settings.Save();
        }

        private void ulFileSelBtn_Click(object sender, EventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.Multiselect = true;//该值确定是否可以选择多个文件 
            dialog.Title = "请选择数据文件";
            dialog.Filter = "数据文件(*.*)|*.*";
            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                this.dataFileTextBox.Text = dialog.FileName;
            }
        }

        private void selLicBtn_Click(object sender, EventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.Multiselect = true;//该值确定是否可以选择多个文件 
            dialog.Title = "请选择 license 文件";
            dialog.Filter = "license文件(*.license)|*.license";
            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                this.licTextBox.Text = dialog.FileName;
            }
        }

        private void initBtn_Click(object sender, EventArgs e)
        {
            callback = new LibCmdRecvCallBack(LibCmdRecv);
            bool logCheck = logCheckbox.Checked;
            string lic = licTextBox.Text;
            int result = -1;
            if(logCheck)
            {
                string path = System.IO.Directory.GetCurrentDirectory();
                result = AmtpcaInit(lic, path, callback);
            }
            else
            {
                result = AmtpcaInit(lic, null, callback);
            }
            
            string tip;
            if (result != 0)
            {
                tip = string.Format("Init AmtpcaDll failed! {0}", dll_result.Parse(result));
                init = false;
            }
            else
            {
                init = true;
                tip = string.Format("Init AmtpcaDll success! {0}", dll_result.Parse(result));
                
            }
            AppendTip(tip);
        }

        private void releaseBtn_Click(object sender, EventArgs e)
        {
            string tip;
            if (init == false)
            {
                tip = string.Format("Init AmtpcaDll has not been init! ");
                AppendTip(tip);
                return;
            }
            init = false;
            AmtpcaRelease();
            tip = string.Format("AmtpcaDll release! ");
            AppendTip(tip);
        }

        private void loginBtn_Click(object sender, EventArgs e)
        {
            Task task = new Task(Login);
            task.Start();
        }
        private void Login()
        {
            string tip;
            if (init == false)
            {
                tip = string.Format("Please init AmtpcaDll first! ");
                AppendTip(tip);
                return;
            }
            AmtpCmd cmd = new AmtpCmd(AmtpCmd.Cmd.login);
            LoginStru stru = new LoginStru("cmdi_wyzx");
            AppendTip(string.Format("Send command Login! "));
            AmtpcaSendCmd(cmd.Value(), stru.Ptr(), true, 3000);
            //AppendTip(string.Format("Send command Login finish! "));

            AmtpCmd resp = new AmtpCmd(AmtpCmd.Cmd.login_resp);
            LoginRespStru resp_s = new LoginRespStru();
            int result = AmtpcaWaitForCmd(resp.Value(), resp_s.Ptr(), 5000);

            tip = string.Format("Wait for login response({0})! resp result = {1}, manufacturer = {2}"
                , result, resp_s.Result(), resp_s.Manufacturer());
            AppendTip(tip);
        }
        private void clearBtn_Click(object sender, EventArgs e)
        {
            tipTextBox.Clear();
        }
        delegate void AppendTipCallBack(string text);
        private void AppendTip(string tipText)
        {
            if (this.tipTextBox.InvokeRequired)
            {
                AppendTipCallBack atcb = new AppendTipCallBack(AppendTip);
                this.Invoke(atcb, new object[] { tipText });
            }
            else
            {
                DateTime dt = DateTime.Now;
                string date_time = dt.ToLocalTime().ToString() + "." + dt.Millisecond.ToString();
                this.tipTextBox.Text += date_time + ":" + tipText + "\n";
                this.tipTextBox.SelectionStart = tipTextBox.Text.Length;

                this.tipTextBox.ScrollToCaret();
            }
        }

        void LibCmdRecv(UInt32 cmd, IntPtr cmd_s, IntPtr reserve)
        {
            //string name = "";
            //if (namePtr != IntPtr.Zero)
            //{
            //    name = Marshal.PtrToStringAnsi(namePtr);
            //}
            //string text = "";
            //if (textPtr != IntPtr.Zero)
            //{
            //    text = Marshal.PtrToStringAnsi(textPtr);
            //}
            //OutputTrace(name, string.Format("{0}({1},{2})", text, p1, p2));
            //string tip;
            //tip = string.Format("LibCmdRecv! ");
            //AppendTip(tip);
            if(cmd == Convert.ToInt32(AmtpCmd.Cmd.logout))
            {
                Task task = new Task(Logout);
                task.Start();
            }
        }
        private void RecvConf()
        {

        }
        private void logoutBtn_Click(object sender, EventArgs e)
        {
            Task task = new Task(Logout);
            task.Start();
        }
        private void Logout()
        {
            string tip;
            if (init == false)
            {
                tip = string.Format("Please init AmtpcaDll first! ");
                AppendTip(tip);
                return;
            }
            AmtpCmd cmd = new AmtpCmd(AmtpCmd.Cmd.logout);
            AppendTip(string.Format("Send command Logout! "));
            AmtpcaSendCmd(cmd.Value(), IntPtr.Zero, true, 3000);
            AmtpCmd resp = new AmtpCmd(AmtpCmd.Cmd.logout_resp);
            LogoutRespStru resp_s = new LogoutRespStru();
            int result = AmtpcaWaitForCmd(resp.Value(), resp_s.Ptr(), 5000);

            tip = string.Format("Wait for logout response({0})! resp result = {1}"
                , result, resp_s.Result());
            AppendTip(tip);
        }
    }
    class DllResult
    {
        Dictionary<int, string> result_dic = new Dictionary<int, string>();
        public DllResult()
        {
            result_dic.Add(0, "amtp success");
            result_dic.Add(-101, "amtp is working");
            result_dic.Add(-102, "amtp no working");
            result_dic.Add(-103, "amtp parameter error");
            result_dic.Add(-104, "amtp open lib failed");
            result_dic.Add(-105, "amtp version failed");
            result_dic.Add(-106, "amtp timeout");
            result_dic.Add(-107, "amtp queuq empty");
            result_dic.Add(-108, "amtp msg is null");
            result_dic.Add(-109, "amtp msg length error");
            result_dic.Add(-110, "amtp send para error");
            result_dic.Add(-111, "amtp license error");
            result_dic.Add(-112, "amtp mq setup error");
            result_dic.Add(-113, "amtp unknown msg");
            result_dic.Add(-114, "amtp send error");
            result_dic.Add(-115, "amtp wait for cmd error");
            result_dic.Add(-116, "amtp no init");

        }
        public string Parse(int value)
        {
            string result;

            if (result_dic.ContainsKey(value) == false)
            {
                result = string.Format("result code {0}.", value);
            }
            else
            {
                result = string.Format("result code {0}, {1}", value, result_dic[value]);
            }

            return result;
        }
    }
    class AmtpCmd
    {
        public enum Cmd
        {
            reserve = 0x00,
            login = 0x00010001,
            login_resp = 0x00020001,
            logout = 0x00030001,
            logout_resp = 0x00040001,
            config = 0x00050001,
            config_resp = 0x00060001,
            config_data = 0x00110001,
            config_data_resp = 0x00120001,
            config_notify = 0x00130001,
            config_notify_resp = 0x00140001,
            upload_file = 0x00150001,
            upload_file_resp = 0x00160001,
            upload_eof = 0x00170001,
            upload_eof_resp = 0x00180001,
            query_data = 0x001B0001,
            query_data_resp = 0x001C0001,
            upload_file_data = 0x001D0001,
            upload_file_data_resp = 0x001E0001,
            module_conf = 0x001F0001,
            module_conf_resp = 0x00200001,
        };
        public AmtpCmd(Cmd cmd)
        {
            this.cmd = cmd;
        }
        private Cmd cmd;
        public UInt32 Value()
        {
            return Convert.ToUInt32(cmd); 
        }
    }
    class LoginStru
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct Stru
        {
            //char manufacturer_pwd[32];
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string manufacturer_pwd;
        }

        Stru s = new Stru();
        const int size = 32;
        byte[] buf = new byte[size];
        IntPtr ptr = Marshal.AllocHGlobal(size);
        public LoginStru(string manufacturer_pwd)
        {
            s.manufacturer_pwd = manufacturer_pwd;
            Marshal.StructureToPtr(s, ptr, true);
            Marshal.Copy(ptr, buf, 0, size);
        }
        public IntPtr Ptr()
        {
            return ptr;
        }
    }
    class LoginRespStru
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct Stru
        {
            //char manufacturer[32];
            //int32_t result;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string manufacturer;
            public Int32 result;
        }

        Stru s;
        const int size = 36;
        byte[] buf = new byte[size];
        IntPtr ptr = Marshal.AllocHGlobal(size);
        public LoginRespStru()
        {
            //s.manufacturer_pwd = manufacturer_pwd;
            //Marshal.StructureToPtr(s, ptr, true);
            //Marshal.Copy(ptr, buf, 0, size);
        }
        public IntPtr Ptr()
        {
            return ptr;
        }
        public string Manufacturer()
        {
            s = (Stru)Marshal.PtrToStructure(ptr, typeof(Stru));
            return s.manufacturer;
        }
        public int Result()
        {
            s = (Stru)Marshal.PtrToStructure(ptr, typeof(Stru));
            return s.result;
        }
    }
    class LogoutRespStru
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct Stru
        {
            //int32_t result;
            public Int32 result;
        }

        Stru s;
        const int size = 4;
        byte[] buf = new byte[size];
        IntPtr ptr = Marshal.AllocHGlobal(size);
        public IntPtr Ptr()
        {
            return ptr;
        }
        public int Result()
        {
            s = (Stru)Marshal.PtrToStructure(ptr, typeof(Stru));
            return s.result;
        }
    }
}
