using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.Text;

public partial class Default2 : System.Web.UI.Page
{
    protected void Page_Load(object sender, EventArgs e)
    {

    }
    public void btnDownload_Click(object sender, System.EventArgs e)
    {
        System.Threading.Thread thread = new System.Threading.Thread(new System.Threading.ThreadStart(LongTask));
        thread.Start();

        Session["State"] = 1;
        OpenProgressBar(this.Page);

    }

    public static void OpenProgressBar(System.Web.UI.Page Page)
    {
        StringBuilder sbScript = new StringBuilder();

        sbScript.Append("<script language='JavaScript' type='text/javascript'>\n");
        sbScript.Append("<!--\n");
        //需要IE5.5以上支持
        //sbScript.Append("window.showModalDialog('Progress.aspx','','dialogHeight: 100px; dialogWidth: 350px; edge: Raised; center: Yes; help: No; resizable: No; status: No;scroll:No;');\n");
        sbScript.Append("window.open('Progress.aspx','', 'height=100, width=350, toolbar =no, menubar=no, scrollbars=no, resizable=no, location=no, status=no');\n");
        sbScript.Append("// -->\n");
        sbScript.Append("</script>\n");

        Page.RegisterClientScriptBlock("OpenProgressBar", sbScript.ToString());
    }
    private void LongTask()
    {
        //模拟长时间任务
        //每个循环模拟任务进行到不同的阶段
        for (int i = 0; i < 11; i++)
        {
            System.Threading.Thread.Sleep(1000);
            //设置每个阶段的state值，用来显示当前的进度
            Session["State"] = i + 1;
        }
        //任务结束
        Session["State"] = 100;

    }

}

