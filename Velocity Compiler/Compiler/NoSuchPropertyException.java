/**
 * Created by IntelliJ IDEA.
 * User: richardl
 * Date: 10/10/2008
 * Time: 09:38:21
 * To change this template use File | Settings | File Templates.
 */
public class NoSuchPropertyException extends Exception
{
    NoSuchPropertyException(Throwable e)
    {
        super(e);
    }

    NoSuchPropertyException(String msg)
    {
        super(msg);
    }

}
